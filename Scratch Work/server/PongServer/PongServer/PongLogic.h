#include <vector>
#include <string>
using namespace std;

#define ARENA_WIDTH 600
#define ARENA_HEIGHT 600

class Paddle {
public:
	Paddle(int ClientID, int startX, int startY, int width, int height) : clientID(ClientID), x(startX), y(startY), width(width), height(height), speed(4), right(false),
	left(false), score(0) {
	}

	void update() {
		x += speed * ((-left)*(right ^ left)); // xor the values of right and left

		if (x < 0) { //all the way to the left
			x = 0;
		}
		else if (x + width > ARENA_WIDTH) { //all the way to the right
			x = ARENA_WIDTH - width;
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
};

class Ball {
public:
	Ball(int startX, int startY) : x(startX), y(startY), xSpeed(0), ySpeed(3), radius(5) {
	}

	void update(vector<Paddle> paddles) {
		x += xSpeed;
		y += ySpeed;
		int topX = x - radius;
		int topY = y - radius;
		int bottomX = x + radius;
		int bottomY = y + radius;

		if (topX < 0) {
			x = radius;
			xSpeed = -xSpeed;
		}
		else if (bottomX > ARENA_WIDTH) {
			x = ARENA_WIDTH - radius;
			xSpeed = -xSpeed;
		}

		if (y < 0) { //this, ofcourse, will need to be changed for Assignment 4
			y = radius;
			ySpeed = -ySpeed;
		}

		if (y > ARENA_HEIGHT) {
			xSpeed = 0;
			ySpeed = 3;
			x = ARENA_WIDTH / 2;
			y = ARENA_HEIGHT / 2;
			paddles[0].score = 0; //this will have to be modified for Assignment 4 but should be fine for now
		}

		for (int i = 0; i < paddles.size(); i++) {
			if (topY < (paddles[i].y + paddles[i].height) && bottomY > paddles[i].y && topX < (paddles[i].x + paddles[i].width) && bottomX > paddles[i].x) {
				ySpeed = -3;
				xSpeed += paddles[i].speed * ((-paddles[i].left)*(paddles[i].right ^ paddles[i].left)) / 2;
				y += ySpeed;
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

	void update() {
		for (int i = 0; i < players.size(); i++) {
			players[i].update();
		}

		ball.update(players);
	}

	string buildGameStateMessage() {
		string message = "";
		for (int i = 0; i < players.size(); i++) {
			message += "p" + to_string(i+1) + "p:" + to_string(players[i].x) + "," + to_string(players[i].y) + "|s" + to_string(i+1) + ":" + to_string(players[i].score) + "|";
		}
		return message + "pb:" + to_string(ball.x) + "," +  to_string(ball.y); //format is p1p:585,21|s1:100|p2p:45,54|s2:1205 ... |pnp:455,34|sn:1020|pb:212,543
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
		players.push_back(Paddle(clientID, ARENA_WIDTH / 2 - 25, ARENA_HEIGHT - 20, 50, 10));//this will need to be changed for Assignment 4
	}

private:	
	vector<Paddle> players;
	Ball ball;
	bool gameStarted;
};