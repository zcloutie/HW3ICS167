#include <vector>
using namespace std;

#define ARENA_WIDTH 600
#define ARENA_HEIGHT 600

class GameState {
public:
	GameState() : ball(ARENA_WIDTH/2, ARENA_HEIGHT/2) {
	}
private:	
	vector<Paddle> players;
	Ball ball;
};

class Paddle {
public:
	Paddle(int startX, int startY, int width, int height) : x(startX), y(startY), width(width), height(height) {
	}

	void update() {
		x += speed * direction;

		if (x < 0) { //all the way to the left
			x = 0;
		}
		else if (x + width > ARENA_WIDTH) { //all the way to the right
			x = ARENA_WIDTH - width;
		}
	}

	int x;
	int y;
    int width = 50;
	int height = 10;
	const int speed = 4;
	int direction = 0; //will be set to 0 for neutral (dont move), -1 for left and 1 for right
	int score = 0;
};

class Ball{
public:
	Ball(int startX, int startY) : x(startX), y(startY){
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

		for (int i = 0; i < paddles.size; i++) {
			if (topY < (paddles[i].y + paddles[i].height) && bottomY > paddles[i].y && topX < (paddles[i].x + paddles[i].width) && bottomX > paddles[i].x) {
				ySpeed = -3;
				xSpeed += paddles[i].speed * paddles[i].direction / 2;
				y += ySpeed;
				paddles[i].score++;
			}
		}
	}

private:
	int x;
	int y;
	const int radius = 5;
	int xSpeed = 0;
	int ySpeed = 3;
};