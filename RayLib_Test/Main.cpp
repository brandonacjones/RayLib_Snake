#include "raylib.h"
#include <vector>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <string>

Color bgColor = GetColor(0x1A4314FF); // Background
Color gridSquareColor = GetColor(0x2C5E1AFF); // Empty grid squares
Color snakeColor = GetColor(0xB2D2A4FF); // Snake body
Color treatColor = GetColor(0x32CD30FF); // Snake treat

const int SCREEN_WIDTH = 1060; // Screen width in px
const int SCREEN_HEIGHT = 1150; // Screen height in px
const int GRID_SIZE = 20; // Number of cells in each direction of grid
const int CELL_SIZE = 50; // Grid square size in px
const int X_OFFSET = 10; // Distance from left edge of window in px
const int Y_OFFSET = 100; // Distance from top edge of window in px

int direction = 0; // Current travel direction [0 = N], [1 = E], [2 = S], [3 = W]

// Grid coordinates of snake treat
int treatX;
int treatY;

// Current score
int score = 0;

// Takes a grid position and axis and returns a pixel value accounting for x or y offset
float gridToPixel(int grid, int axis) {
	if (axis == 0) { // X
		return ((grid * CELL_SIZE) + (grid * 2) + X_OFFSET);
	}
	else { // Y
		return ((grid * CELL_SIZE) + (grid * 2) + Y_OFFSET);
	}
	
}

// structure holding each discrete segment of the snake.
struct snakeSegment {
	int x; // Grid Space
	int y; // Grid Space

	snakeSegment() : x(10), y(10) {}; // Default constructor creates segment at the middle of the grid
	snakeSegment(int x, int y) : x(x), y(y) {}; // Parameterized constructor creates segment at specified gridsquare
};

// Vector of snake segments, snake[0] represents the head of the snake
std::vector<snakeSegment> snake = { snakeSegment() };

// Draws all empty grid squares
void drawGrid() {
	for (int x = 0; x < GRID_SIZE; ++x) {
		for (int y = 0; y < GRID_SIZE; ++y) {
			DrawRectangle((y * CELL_SIZE) + (y * 2) + X_OFFSET, (x * CELL_SIZE) + (x * 2) + Y_OFFSET, CELL_SIZE, CELL_SIZE, gridSquareColor);
		}
	}
}

// Draws each segment of the snake
void drawSnake() {
	for (snakeSegment& segment : snake) {
		DrawRectangle(gridToPixel(segment.x, 0), gridToPixel(segment.y, 1), CELL_SIZE, CELL_SIZE, snakeColor);
	}
}

// Listens for an arrow key press and updates travel direction
void updateDirection() {
	if (IsKeyPressed(KEY_UP) && direction != 2) { 
		direction = 0; // North
	}
	if (IsKeyPressed(KEY_RIGHT) && direction != 3) {
		direction = 1; // East
	}
	if (IsKeyPressed(KEY_DOWN) && direction != 0) {
		direction = 2; // South
	}
	if (IsKeyPressed(KEY_LEFT) && direction != 1) {
		direction = 3; // West
	}
}

// Selects a random empty grid square for treat placement
void spawnTreat() {
	bool validTreatLoc = false;
	while (!validTreatLoc) {
		validTreatLoc = true;
		treatX = std::rand() % GRID_SIZE;
		treatY = std::rand() % GRID_SIZE;
		for (snakeSegment segment : snake) {
			if (treatX == segment.x || treatY == segment.y) {
				validTreatLoc = false;
			}
		}
	}
	
}

// Updates snake position
void updateSnake() {
	int snakeSize = snake.size() - 1; // Index of the snake tail
	
	// Integers to store tail position
	int tailX;
	int tailY;

	// Head position
	int headX = snake[0].x;
	int headY = snake[0].y;

	bool treatFound = false; // If the treat has been found

	// Check if snake head is overlapping the treat
	if (snake[0].x == treatX && snake[0].y == treatY) {

		// Store the current tail position
		tailX = snake[snakeSize].x;
		tailY = snake[snakeSize].y;
		treatFound = true; // Treat has been found
		score += 1; // Increment the score
	}

	// Iterate over all snake segments
	for (int i = snakeSize; i >= 0; --i) {

		// Check if snake head is overlapping a body segment
		if (snake[i].x == headX && snake[i].y == headY && i != 0) {
			
			// Reset snake body to just the head
			snake.clear();
			snake.push_back(snakeSegment());

			direction = 0; // Reset travel direction
			score = 0; // Reset score
			break;
		}

		// Move each body segment to the space of the body segment in front of it
		if (i != 0) {
			snake[i].x = snake[i - 1].x;
			snake[i].y = snake[i - 1].y;
		}

		// Move the head of the snake to the next space in the travel direction
		else {
			switch (direction) {
			case(0): // North
				if (snake[i].y == 0) {
					snake[i].y = GRID_SIZE - 1;
				}
				else {
					snake[i].y -= 1;
				}
				break;
			case(1): // East
				if (snake[i].x == GRID_SIZE - 1) {
					snake[i].x = 0;
				}
				else {
					snake[i].x += 1;
				}
				break;
			case(2): // South
				if (snake[i].y == GRID_SIZE - 1) {
					snake[i].y = 0;
				}
				else {
					snake[i].y += 1;
				}
				break;
			case(3): // West
				if (snake[i].x == 0) {
					snake[i].x = GRID_SIZE - 1;
				}
				else {
					snake[i].x -= 1;
				}
				break;
			}
		}
	}
	// Add a new body segment at the old tail position if treat was found, then update treat position
	if (treatFound) {
		snake.push_back(snakeSegment(tailX, tailY));
		spawnTreat();
	}
}

// Draw the treat
void drawTreat() {
	DrawRectangle(gridToPixel(treatX, 0), gridToPixel(treatY, 1), CELL_SIZE, CELL_SIZE, treatColor);
}

int main() {
	// Initialize
	std::srand(static_cast<unsigned int>(std::time(nullptr))); // Seed the random generator for treat generation
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake"); // Create the window
	SetTargetFPS(4); // Set framerate
	spawnTreat(); // Spawn initial treat

	// Game Loop
	while (!WindowShouldClose()) {

		updateDirection(); // Check if direction should be changed
		updateSnake(); // Update snake position

		// Draw game elements
		BeginDrawing();
			
		ClearBackground(bgColor); // Clear last frame

		// Draw the current score
		std::string scoreText = "SCORE " + std::to_string(score);
		DrawText(scoreText.c_str(), 50, 50, 50, WHITE);

		drawGrid();
		drawSnake();
		drawTreat();

		EndDrawing();
	}

	CloseWindow();

	return 0;
}
