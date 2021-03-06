#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include "websocket.h"

using namespace std;

#define INTERVAL_MS 10

webSocket server;

int interval_clocks = CLOCKS_PER_SEC * INTERVAL_MS / 1000;

/* called when a client connects */
void openHandler(int clientID){
    ostringstream os;
    os << "Stranger " << clientID << " has joined.";

    vector<int> clientIDs = server.getClientIDs();
    for (int i = 0; i < clientIDs.size(); i++){
        if (clientIDs[i] != clientID)
            server.wsSend(clientIDs[i], os.str());
    }
    server.wsSend(clientID, "Welcome!");
}

/* called when a client disconnects */
void closeHandler(int clientID){
    ostringstream os;
    os << "Stranger " << clientID << " has left.";

    vector<int> clientIDs = server.getClientIDs();
    for (int i = 0; i < clientIDs.size(); i++){
        if (clientIDs[i] != clientID)
            server.wsSend(clientIDs[i], os.str());
    }
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
	if (message.substr(0, 2) == "ID") { 
		server.wssetClientCIDs(clientID, message.substr(3, message.size())); //client sent "ID:name"
		string names = "";
		for (int i = 0; i < clientIDs.size(); i++) {
			names += "n" + to_string(i+1) + ":" + server.getwsClientName(i) + "|";
		}
		for (int i = 0; i < clientIDs.size(); i++) {
			server.wsSend(clientIDs[i], names);
		}
	}
	else if (message.substr(0, 2) == "LD") { //When left button is pushed down
		server.gameState.setClientLeft(clientID, true);
	}

	else if (message.substr(0, 2) == "LU") { //When left button is released
		server.gameState.setClientLeft(clientID, false);
	}

	else if (message.substr(0, 2) == "RD") { //When right button is pushed down
		server.gameState.setClientRight(clientID, true);
	}

	else if (message.substr(0, 2) == "RU") { //When left button is released
		server.gameState.setClientRight(clientID, false);
	}
}

/* called once per select() loop */
void periodicHandler(){
	static clock_t oldTime = clock();// +interval_clocks;
    clock_t newTime = clock();
    if (newTime >= oldTime + interval_clocks){
		/*
        ostringstream os;
		//Deprecated ctime API in Windows 10
		char timecstring[26];
		ctime_s(timecstring, sizeof(timecstring), &current);
		string timestring(timecstring);
        timestring = timestring.substr(0, timestring.size() - 1);
        os << timestring;

        vector<int> clientIDs = server.getClientIDs();
        for (int i = 0; i < clientIDs.size(); i++)
            server.wsSend(clientIDs[i], os.str());
		*/
		server.gameState.update(newTime - oldTime);

		vector<int> clientIDs = server.getClientIDs();
		for (int i = 0; i < clientIDs.size(); i++) {
			server.wsSend(clientIDs[i], server.gameState.buildGameStateMessage());
		}

        oldTime = clock();
    }
}

int main(int argc, char *argv[]){
    int port;

    cout << "Please set server port: ";
    cin >> port;

    /* set event handler */
    server.setOpenHandler(openHandler);
    server.setCloseHandler(closeHandler);
    server.setMessageHandler(messageHandler);
    server.setPeriodicHandler(periodicHandler);

    /* start the chatroom server, listen to ip '127.0.0.1' and port '8000' */
    server.startServer(port);

    return 1;
}
