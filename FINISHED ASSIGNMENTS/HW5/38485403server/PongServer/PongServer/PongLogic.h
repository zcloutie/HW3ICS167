#include <vector>
#include <string>
#include <chrono>
using namespace std;

#define ARENA_WIDTH 600
#define ARENA_HEIGHT 600
#define PADDLE_WIDTH 50
#define PADDLE_HEIGHT 10

class Paddle {
public:
	Paddle(int playerNumber, int ClientID, int startX, int startY, int width, int height) : playerNum(playerNumber), clientID(ClientID), x(startX), y(startY), width(width), height(height), speed(4), 
		right(false),left(false), score(0) {
	}

	void update(float deltaTime) {
		switch (playerNum) {
			case 1: //ommited the break statement here so that the case will be skipped over since player 1 and 2's movement logic is identical
			case 2:
				x += speed * ((left ? -left : true)*(right ^ left)) * (deltaTime / 10.0); // xor the values of right and left

				if (x < 0) { //all the way to the left
					x = 0;
				}
				else if (x + width > ARENA_WIDTH) { //all the way to the right
					x = ARENA_WIDTH - width;
				}
				break;
			case 3://same thing as above break statement comment
			case 4:
				y += speed * ((left ? -left : true)*(right ^ left)) * (deltaTime / 10.0); // xor the values of right and left

				if (y < 0) { //all the way to the top
					y = 0;
				}
				else if (y + height > ARENA_HEIGHT) { //all the way to the bottom
					y = ARENA_HEIGHT - height;
				}
				break;
		}
	}

	void setLeft(bool isDown) {
		left = isDown;
	}

	void setRight(bool isDown) {
		right = isDown;
	}

	int x;
	int y;
	int width = 50;
	int height = 10;
	const int speed = 4;
	bool right = false;
	bool left = false;
	int score = 0;
	int clientID;
	int playerNum;
};

class Ball {
public:
	Ball(int startX, int startY) : x(startX), y(startY), xSpeed(0), ySpeed(3), radius(5) {
	}

	void update(float deltaTime, vector<Paddle>& paddles) {
		x += xSpeed * (deltaTime / 10.0);
		y += ySpeed * (deltaTime / 10.0);
		int topX = x - radius;
		int topY = y - radius;
		int bottomX = x + radius;
		int bottomY = y + radius;

		if (y < 0) {
			xSpeed = 0;
			ySpeed = -3;
			x = ARENA_WIDTH / 2;
			y = ARENA_HEIGHT / 2;
			paddles[1].score = 0; //set the score for the second player (top) to 0
		}

		if (y > ARENA_HEIGHT) {
			xSpeed = 0;
			ySpeed = 3;
			x = ARENA_WIDTH / 2;
			y = ARENA_HEIGHT / 2;
			paddles[0].score = 0; //set the score for the first player (bottom) to 0
		}

		if (x < 0) {
			xSpeed = -3;
			ySpeed = 0;
			x = ARENA_WIDTH / 2;
			y = ARENA_HEIGHT / 2;
			paddles[3].score = 0; //set the score for the fourth player (left) to 0
		}

		if (x > ARENA_WIDTH) {
			xSpeed = 3;
			ySpeed = 0;
			x = ARENA_WIDTH / 2;
			y = ARENA_HEIGHT / 2;
			paddles[2].score = 0; //set the score for the third player (right) to 0
		}

		for (int i = 0; i < paddles.size(); i++) {
			if (topY < (paddles[i].y + paddles[i].height) && bottomY > paddles[i].y && topX < (paddles[i].x + paddles[i].width) && bottomX > paddles[i].x) {
				switch (i) {
					case 0: //bottom player
						ySpeed = -3;
						xSpeed += paddles[i].speed * ((paddles[i].left ? -paddles[i].left : true)*(paddles[i].right ^ paddles[i].left)) / 2;//+ ((paddles[i].x + paddles[i].width / 2) - x) / 2;
						y += ySpeed * (deltaTime / 10.0);
						break;
					case 1: //top player
						ySpeed = 3;
						xSpeed += paddles[i].speed * ((paddles[i].left ? -paddles[i].left : true)*(paddles[i].right ^ paddles[i].left)) / 2;//+ ((paddles[i].x + paddles[i].width / 2) - x) / 2;
						y += ySpeed * (deltaTime / 10.0);
						break;
					case 2: //right player
						xSpeed = -3;
						ySpeed += paddles[i].speed * ((paddles[i].left ? -paddles[i].left : true)*(paddles[i].right ^ paddles[i].left)) / 2;//+ ((paddles[i].y + paddles[i].height / 2) - y) / 2;
						x += xSpeed * (deltaTime / 10.0);
						break;
					case 3: //left player
						xSpeed = 3;
						ySpeed += paddles[i].speed * ((paddles[i].left ? -paddles[i].left : true)*(paddles[i].right ^ paddles[i].left)) / 2;//+ ((paddles[i].y + paddles[i].height / 2) - y) / 2;
						x += xSpeed * (deltaTime / 10.0);
						break;
				}
				paddles[i].score++;
			}
		}
	}

	int x;
	int y;
private:
	const int radius = 5;
	int xSpeed = 0;
	int ySpeed = 3;
};

class GameState {
public:
	GameState() : ball(ARENA_WIDTH / 2, ARENA_HEIGHT / 2), players{}, gameStarted(false) {
	}

	bool isGameStarted() {
		return gameStarted;
	}

	void startGame() {
		gameStarted = true;
	}

	void stopGame() {
		gameStarted = false;
	}

	void update(int deltaTime) {
		for (int i = 0; i < players.size(); i++) {
			players[i].update(deltaTime);
		}

		ball.update(deltaTime, players);
	}

	string buildGameStateMessage() {
		string message = to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count()) + "|";
		for (int i = 0; i < players.size(); i++) {
			message += "p" + to_string(i+1) + "p:" + to_string(players[i].x) + "," + to_string(players[i].y) + "|s" + to_string(i+1) + ":" + to_string(players[i].score) + "|";
		}
		return message + "bp:" + to_string(ball.x) + "," +  to_string(ball.y); //format is p1p:585,21|s1:100|p2p:45,54|s2:1205 ... |pnp:455,34|sn:1020|bp:212,543
	}
	
	void setClientLeft(int clientID, bool isDown) {
		for (int i = 0; i < players.size(); i++) {
			if (players[i].clientID == clientID) {
				players[i].setLeft(isDown);
				break;
			}
		}
	}

	void setClientRight(int clientID, bool isDown) {
		for (int i = 0; i < players.size(); i++) {
			if (players[i].clientID == clientID) {
				players[i].setRight(isDown);
				break;
			}
		}
	}

	void addPlayer(int clientID) {
		switch (players.size()) {
			case 0:
				players.push_back(Paddle(1, clientID, ARENA_WIDTH / 2 - PADDLE_WIDTH / 2, ARENA_HEIGHT - 2 * PADDLE_HEIGHT, PADDLE_WIDTH, PADDLE_HEIGHT));//bottom
				break;
			case 1:
				players.push_back(Paddle(2, clientID, ARENA_WIDTH / 2 - PADDLE_WIDTH / 2, PADDLE_HEIGHT, PADDLE_WIDTH, PADDLE_HEIGHT));//top
				break;
			case 2:
				players.push_back(Paddle(3, clientID, ARENA_WIDTH - 2 * PADDLE_HEIGHT, ARENA_HEIGHT / 2 - PADDLE_WIDTH / 2, PADDLE_HEIGHT, PADDLE_WIDTH));//right
				break;
			case 3:
				players.push_back(Paddle(4, clientID, PADDLE_HEIGHT, ARENA_HEIGHT / 2 - PADDLE_WIDTH / 2, PADDLE_HEIGHT, PADDLE_WIDTH));//left
				break;
		}
	}

private:	
	vector<Paddle> players;
	Ball ball;
	bool gameStarted;
};