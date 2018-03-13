#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include "websocket.h"
#include <random>

using namespace std;

#define INTERVAL_MS 10

webSocket server;

vector<pair<int, pair<string, int>>> inQueue{};
vector<pair<int, pair<string, int>>> outQueue{};

vector<pair<int, int>> pendingAcks{};

int lastMessageID = 0;

int latency = 0;
long estimatedLatency = 0;

vector<long long> estimatedLatencies{ 0,0,0,0 };

vector<int> latencies{0,0,0,0};

int latencyAccelleration = 1;

int latencyType = -1;

int minLatency = 0;
int maxLatency = 0;

random_device device;

default_random_engine randEngine(device());

uniform_int_distribution<int> uniDistribution;

int interval_clocks = CLOCKS_PER_SEC * INTERVAL_MS / 1000;

void enqueInput(int clientID, string message, int expectedTime) {
	inQueue.push_back(make_pair(expectedTime, make_pair(message, clientID)));
}

int produceNextLatency(int clientIndex) {

	switch (latencyType) {
	case 1:
		break;
	case 2:
		latencies[clientIndex] = uniDistribution(randEngine);
		break;
	case 3:
		if (latencies[clientIndex] + latencyAccelleration < maxLatency) {
			latencies[clientIndex] += latencyAccelleration;
		}
		else {
			latencies[clientIndex] = maxLatency;
		}
		break;
	}

	return latencies[clientIndex];
}

void processInput(int currentTime) {
	for (int i = 0; i < inQueue.size(); i++) {
		if (inQueue[i].first <= currentTime) {
			pair<string, int> message = inQueue[i].second;

			vector<int> clientIDs = server.getClientIDs();
			/*
			long long num = 0;
			istringstream(message.first.substr(0, message.first.find("|"))) >> num;
			long long currTime = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
			for (int i = 0; i < clientIDs.size(); i++) {
				if (clientIDs[i] == message.second) {
					estimatedLatencies[i] = currTime - num;
					break;
				}
			}*/
			
			if (message.first.substr(0, 3) == "ID") {
				server.wssetClientCIDs(message.second, message.first.substr(3, message.first.size())); //client sent "ID:name"
				string names = "";
				for (int i = 0; i < clientIDs.size(); i++) {
					names += "n" + to_string(i + 1) + ":" + server.getwsClientName(i) + "|";
				}
				for (int i = 0; i < clientIDs.size(); i++) {
					server.wsSend(clientIDs[i], names);
				}
			}
			else if (message.first.substr(0, 3) == "AK") {
				int id;
				istringstream(message.first.substr(3, message.first.size())) >> id;

				for (int i = 0; i < pendingAcks.size(); i++) {
					if (pendingAcks[i].first == id) {
						for (int i = 0; i < clientIDs.size(); i++) {
							if (clientIDs[i] == message.second) {
								estimatedLatencies[i] = currentTime - pendingAcks[i].second;
								break;
							}
						}
						pendingAcks.erase(find(pendingAcks.begin(), pendingAcks.end(), pendingAcks[i]));
						break;
					}
				}
			}
			else if (message.first.substr(0, 3) == "LD") { //When left button is pushed down
				server.gameState.setClientLeft(message.second, true);
			}

			else if (message.first.substr(0, 3) == "LU") { //When left button is released
				server.gameState.setClientLeft(message.second, false);
			}

			else if (message.first.substr(0, 3) == "RD") { //When right button is pushed down
				server.gameState.setClientRight(message.second, true);
			}

			else if (message.first.substr(0, 3) == "RU") { //When left button is released
				server.gameState.setClientRight(message.second, false);
			}

			inQueue.erase(find(inQueue.begin(), inQueue.end(), inQueue[i]));
		}
	}
}

void enqueOutput(int clientID, string message, int expectedTime) {
	outQueue.push_back(make_pair(expectedTime, make_pair(message, clientID)));
}

void sendOutput(int currentTime) {
	for (int i = 0; i < outQueue.size(); i++) {
		if (outQueue[i].first <= currentTime) {
			pair<string, int> message = outQueue[i].second;

			server.wsSend(message.second, message.first);

			outQueue.erase(find(outQueue.begin(), outQueue.end(), outQueue[i]));
		}
	}
}

/* called when a client connects */
void openHandler(int clientID){
	/*
    ostringstream os;
    os << "Stranger " << clientID << " has joined.";

    vector<int> clientIDs = server.getClientIDs();
    for (int i = 0; i < clientIDs.size(); i++){
        if (clientIDs[i] != clientID)
            server.wsSend(clientIDs[i], os.str());
    }
    server.wsSend(clientID, "Welcome!");*/
}

/* called when a client disconnects */
void closeHandler(int clientID){
	/*
    ostringstream os;
    os << "Stranger " << clientID << " has left.";

    vector<int> clientIDs = server.getClientIDs();
    for (int i = 0; i < clientIDs.size(); i++){
        if (clientIDs[i] != clientID)
            server.wsSend(clientIDs[i], os.str());
    }*/
	server.gameState.stopGame();
}

/* called when a client sends a message to the server */
void messageHandler(int clientID, string message){
    /*ostringstream os;
    os << "Stranger " << clientID << " says: " << message;

    vector<int> clientIDs = server.getClientIDs();
    for (int i = 0; i < clientIDs.size(); i++){
        if (clientIDs[i] != clientID)
            server.wsSend(clientIDs[i], os.str());
    }*/
	vector<int> clientIDs = server.getClientIDs();
	for (int i = 0; i < clientIDs.size(); i++) {
		if (clientIDs[i] == clientID) {
			enqueInput(clientID, message, clock() + produceNextLatency(i));
		}
	}
}

/* called once per select() loop */
void periodicHandler(){
	static clock_t oldTime = clock();// +interval_clocks;
    clock_t newTime = clock();
    if (newTime >= oldTime + interval_clocks){
		processInput(newTime);

		server.gameState.update(newTime - oldTime);

		vector<int> clientIDs = server.getClientIDs();
		for (int i = 0; i < clientIDs.size(); i++) {
			pendingAcks.push_back(make_pair(lastMessageID, newTime));
			enqueOutput(clientIDs[i], to_string(lastMessageID++) + server.gameState.buildGameStateMessage(), newTime + produceNextLatency(i));
			//cout << estimatedLatencies[i] << endl;
		}

		sendOutput(newTime);

        oldTime = clock();
    }
}



int main(int argc, char *argv[]){
    int port;

    cout << "Please set server port: ";
    cin >> port;

	cout << "Please select latency type:\n 1: Fixed\n 2: Random\n 3: Incremental\n";
	cin >> latencyType;


	switch (latencyType) {
		case 1:
			cout << "Please enter the amount of desired latency: ";
			cin >> latencies[0], latencies[1], latencies[2], latencies[3];
			break;
		case 2:
			cout << "Please enter the minimum desired latency: ";
			cin >> minLatency;

			cout << "Please enter the maximum desired latency: ";
			cin >> maxLatency;

			uniDistribution.param(uniform_int_distribution<int>::param_type(minLatency, maxLatency));
			break;
		case 3:
			cout << "Please enter the minimum desired latency: ";
			cin >> minLatency, latencies[0], latencies[1], latencies[2], latencies[3];

			cout << "Please enter the maximum desired latency: ";
			cin >> maxLatency;
			break;
	}
	

    /* set event handler */
    server.setOpenHandler(openHandler);
    server.setCloseHandler(closeHandler);
    server.setMessageHandler(messageHandler);
    server.setPeriodicHandler(periodicHandler);

    /* start the chatroom server, listen to ip '127.0.0.1' and port '8000' */
    server.startServer(port);

    return 1;
}
