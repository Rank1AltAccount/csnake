#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SNAKE_HEAD_CHAR 'O'
#define SNAKE_BODY_CHAR 'o'
#define SPACE_CHAR ' '
#define WALL_CHAR '#'
#define FOOD_CHAR '@'

// Not all coordinates are supposed to be vec2d,
// it is used only where necessary to avoid memory management
typedef struct _vec2d {
	int x;
	int y;
} vec2d;

const int WIDTH = 20;
const int HEIGHT = WIDTH / 2;

char frameBuffer[WIDTH][HEIGHT];

int snakeSize = 0; 

vec2d foodPos;
vec2d snake[WIDTH * HEIGHT];

// Distance between two points measured in right angles
int manhattanDistance(const int x1, const int y1, const int x2, const int y2) {
	int xDiff = x2 - x1;
	int yDiff = y2 - y1;

	// Didn't want to include math.h just for abs()
	if (xDiff < 0) {
		xDiff = -xDiff;
	}

	if (yDiff < 0) {
		yDiff = -yDiff;
	}

	return xDiff + yDiff;
}

int randInRange(const int lower, const int upper) {
	return (rand() % (upper - lower + 1)) + lower;
}

// Tell your CPU I'm sorry 
void wait(const int seconds) {
	time_t goal = time(0) + seconds;
	time_t now;

	while (time(&now) < goal);
}

void clearConsole() {
	int i;

	for (i = 0; i < 48; i++) {
		putchar('\n');
	}
}

void clearBuffer() {
	int x;
	int y;

    	memset(frameBuffer, WALL_CHAR, sizeof(frameBuffer)); 

	for (y = 1; y < HEIGHT - 1; y++) {
		for (x = 1; x < WIDTH - 1; x++) {
			frameBuffer[x][y] = SPACE_CHAR;
		}
	} 
}
 
void drawFrame() {
	int x;
	int y;

	for (y = 0; y < HEIGHT; y++) {
		for (x = 1; x <= WIDTH; x++) {
			// x - 1 to avoid printing a new line on x == 0
			putchar(frameBuffer[x - 1][y]);

			if (x == WIDTH) {
				putchar('\n');
			}
		}
	}
}

void lose() {
	printf("Game lost!");

	wait(3);

	exit(0);
}

int checkCollision(const int x, const int y) {
	// Collides with bounds
	if (x >= WIDTH - 1 || x <= 0
	|| y >= HEIGHT - 1 || y <= 0) {
		return 1;
	} 

	int i;

	// Collides with body
	for (i = 0; i < snakeSize; i++) {
		if (x == snake[i].x && y == snake[i].y) {
			return 1;
		}
	}

	return 0;
}

void createFood() {
	int xfood = randInRange(1, WIDTH - 2);
	int yfood = randInRange(1, HEIGHT - 2);

	if (checkCollision(xfood, yfood)) {
		createFood();
	}

	foodPos.x = xfood;
	foodPos.y = yfood;
}

void snakeAddPart(const int x, const int y) { 
	snake[snakeSize] = (vec2d){ .x = x, .y = y }; 

	snakeSize++;
} 
 
void snakeMove(const int xmove, const int ymove) {
	const int xNewHead = snake[0].x + xmove;
	const int yNewHead = snake[0].y + ymove;

	if (checkCollision(xNewHead, yNewHead)) {
		lose();
	}

	// Food eaten
	if (xNewHead == foodPos.x && yNewHead == foodPos.y) {
		snakeAddPart(snake[snakeSize].x, snake[snakeSize].y);

		createFood();
	}

	int i;

	for (i = snakeSize - 1; i >= 1; i--) {
		snake[i].x = snake[i - 1].x;
		snake[i].y = snake[i - 1].y;
	}

	snake[0].x = xNewHead;
	snake[0].y = yNewHead;
}

void snakeThink() {
	const vec2d destinations[4] = {
		{snake[0].x + 1, snake[0].y},
		{snake[0].x - 1, snake[0].y},
		{snake[0].x, snake[0].y + 1},
		{snake[0].x, snake[0].y - 1}
	};

	vec2d bestMove; 

	int bestDistance = -1;
	int i;

	// Determine which of the four directions is closer to food
	// and does not collide with anything
	for (i = 0; i < 4; i++) {
		int dist = manhattanDistance(destinations[i].x, destinations[i].y, foodPos.x, foodPos.y);

		if (dist < bestDistance || bestDistance == -1) {
			if (!checkCollision(destinations[i].x, destinations[i].y)) {
				bestDistance = dist;
				bestMove = destinations[i];
			}
		}
	}
 
	if (bestDistance != -1) { 
		snakeMove(bestMove.x - snake[0].x, bestMove.y - snake[0].y);
	}
	// No valid moves
	else {
		lose();
	}
}

// Write game objects to buffer
void gameToBuffer() {
	frameBuffer[foodPos.x][foodPos.y] = FOOD_CHAR;

	int i;

	for (i = 0; i < snakeSize; i++) {
		if (i == 0) { 
			frameBuffer[snake[i].x][snake[i].y] = SNAKE_HEAD_CHAR;
		}
		else { 
			frameBuffer[snake[i].x][snake[i].y] = SNAKE_BODY_CHAR;
		}
	} 
}

void init() { 
	// Seed the generator
	srand(time(0));

	snakeAddPart(randInRange(1, WIDTH - 1), randInRange(1, HEIGHT - 1)); 
	snakeAddPart(snake[0].x, snake[0].y); 
	snakeAddPart(snake[0].x, snake[0].y); 
  
	createFood(); 
}

void tick() {
	while (1) {
		clearConsole();
		clearBuffer();

		snakeThink();
		gameToBuffer(); 

		drawFrame(); 

		wait(1); 
	} 
}
 
int main() { 
	init();
	tick();
  
	return 0;
}
