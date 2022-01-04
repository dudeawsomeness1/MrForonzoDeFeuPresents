//\  /\	McClain Jorgensen
//\\//\\ The fantastic Mr. Foronzo De Feu (tiger mouse) presents... Raven's Deep

#include "src\graphics\window.h"
#include <GLFW\glfw3.h>
#include <iostream>
#include <conio.h>
#include <iomanip>
#include <Windows.h>
#include <fstream>
#include <string>

#define WORLD_ROWS 25
#define WORLD_COLS 25

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77

#define VERSION "1.3.1"

using namespace std;

//const string VERSION = "1.3.1";

// Tiles
const char TILE = 177; // Ew, global :(
const char PLAYER = '$';
const char ENEMY = 157;
const char ENEMYMOVED = 158;
const char POWUP_EXPLD = '@';	// Explosion all around the player
const char POWUP_LNGSPR = '\\';	// Double length spear

// 15 = white
void setColor(unsigned short color) {
	HANDLE hcon = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hcon, color);
}

void gotoXY(int column, int line)
{
	COORD coord;
	coord.X = column;
	coord.Y = line;
	SetConsoleCursorPosition(
		GetStdHandle(STD_OUTPUT_HANDLE),
		coord
	);
}

void update_screen(const int ROWS, const int COLS, char world[][WORLD_COLS], char healthPoints[], int hp, const int MAXHP, int level) { // WIP
	gotoXY(7, 0); // Level: _
	cout << level;

	// Output health
	gotoXY(4, 1); // HP: _
	if (hp > MAXHP) { // doesn't allow hp to exceed max and go out of bounds. Doesn't change the actual hp though.
		hp = MAXHP;
	}

	for (int i = 0; i < hp; i++) {
		setColor(10); // Green
		cout << healthPoints[i];
	}
	for (int i = hp; i < MAXHP; i++) {
		setColor(12); // Red
		cout << healthPoints[i];
	}
	setColor(15);
	cout << endl;

	// Check for differences and update them.
	char temp;

	for (int i = 0; i < ROWS; i++) {
		int j = 0;
		temp = _getch();
		do {
			if (temp != world[i][j]) { // if char at (j, i) is different than what is on screen update it.
				if (world[i][j] == POWUP_EXPLD) {
					setColor(6);
					cout << world[i][j];
				}
				else if (world[i][j] == POWUP_LNGSPR) {
					setColor(11);
					cout << world[i][j];
				}
				else if ((world[i][j] == -37) || (world[i][j] == '#')) {
					setColor(12); // Red
					cout << world[i][j];
				}
				else {
					setColor(15);
					cout << world[i][j];
				}
			}

			temp = _getch();
			j++;
		} while (temp != '\n');
	}

	//for (int i = 0; i < ROWS; i++) {
	//	for (int j = 0; j < COLS; j++) {
	//		gotoXY(j, i + 2);
	//		for (int boi = 0; boi < ROWS; boi++) {
	//			temp = _getch();
	//		}
	//		
	//		if (temp != world[i][j]) { // if char at (j, i) is different than what is on screen update it.
	//			gotoXY(j, i + 2);
	//			if (world[i][j] == POWUP_EXPLD) {
	//				setColor(6);
	//				cout << world[i][j];
	//			}
	//			else if (world[i][j] == POWUP_LNGSPR) {
	//				setColor(11);
	//				cout << world[i][j];
	//			}
	//			else if ((world[i][j] == -37) || (world[i][j] == '#')) {
	//				setColor(12); // Red
	//				cout << world[i][j];
	//			}
	//			else {
	//				setColor(15);
	//				cout << world[i][j];
	//			}
	//		}
	//	}
	//}
}

void output_world(const int ROWS, const int COLS, char world[][WORLD_COLS], char healthPoints[], int hp, const int MAXHP, int level) {
	system("CLS"); // Clear screen

	cout << "Level: " << level << endl;

	// Output health
	if (hp > MAXHP) { // doesn't allow hp to exceed max and go out of bounds. Doesn't change the actual hp though.
		hp = MAXHP;
	}

	cout << "HP: ";
	for (int i = 0; i < hp; i++) {
		setColor(10); // Green
		cout << healthPoints[i];
	}
	for (int i = hp; i < MAXHP; i++) {
		setColor(12); // Red
		cout << healthPoints[i];
	}
	cout << endl;
	
	// Output world
	setColor(15);
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			if (world[i][j] == POWUP_EXPLD) {
				setColor(6);
				cout << world[i][j];
			}
			else if (world[i][j] == POWUP_LNGSPR) {
				setColor(11);
				cout << world[i][j];
			}
			else if ((world[i][j] == -37) || (world[i][j] == '#')) {
				setColor(12);
				cout << world[i][j];
			}
			else {
				setColor(15);
				cout << world[i][j];
			}
		}
		cout << endl;
	}
}

// Checks if an "object" is already in that row and column.
bool occupied(char world[][WORLD_COLS], int row, int col) {
	if ((world[row][col] == ENEMY) || (world[row][col] == PLAYER) || (world[row][col] == ENEMYMOVED) || (world[row][col] == POWUP_EXPLD) || (world[row][col] == POWUP_LNGSPR)) {
		return true;
	}
	return false;
}

// Checks if that row and column are in the world bounds.
bool isInWorldBounds(int row, int col, const int ROWS, const int COLS) {
	if ((row < 0) || (col < 0)) {
		return false;
	}
	if ((row >= ROWS) || (col >= COLS)) {
		return false;
	}
	return true;
}

bool fight_check(char world[][WORLD_COLS], const int ROWS, const int COLS, int playerRow, int playerCol) {
	for (int i = playerRow - 1; i <= playerRow + 1; i++) { // Check in a 3 by 3 grid around the player for an enemy.
		while (i < 0)
			i++;
		if (i < ROWS) {
			for (int j = playerCol - 1; j <= playerCol + 1; j++) {
				while (j < 0)
					j++;
				if (j < COLS) {
					if ((world[i][j] == ENEMY) || (world[i][j] == ENEMYMOVED))
						return true;
				}
			}
		}
	}
	return false;
}

// Gets the coordinates of the player.
void player_finder(const int ROWS, const int COLS, char world[][WORLD_COLS], int& playerRow, int& playerCol) {
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			if (world[i][j] == PLAYER) {
				playerRow = i;
				playerCol = j;
			}
		}
	}
}

void path_find(char world[][WORLD_COLS], const int ROWS, const int COLS, int& enemyRow, int& enemyCol, int prevEnemyRow, int prevEnemyCol) {
	if (occupied(world, enemyRow, enemyCol)) {
		for (int i = prevEnemyRow - 1; i < prevEnemyRow + 1; i++) { // check for a free spot around the original position
			for (int j = prevEnemyCol - 1; j < prevEnemyCol + 1; j++) {
				if (world[i][j] == TILE) { // Move to free space
					enemyRow = i;
					enemyCol = j;
				}
				else { // Else stay
					enemyRow = prevEnemyRow;
					enemyCol = prevEnemyCol;
				}
			}
		}
	}
}

void reset_moved_enemies(char world[][WORLD_COLS], const int ROWS, const int COLS) {
	for (int i = 0; i < ROWS; i++) { // Reset moved enemies
		for (int j = 0; j < COLS; j++) {
			if (world[i][j] == ENEMYMOVED) {
				world[i][j] = ENEMY;
			}
		}
	}
}

void enemy_ai(char world[][WORLD_COLS], const int ROWS, const int COLS, int& hp, char healthPoints[], const int MAXHP, int level) {
	int playerRow = 0;
	int playerCol = 0;
	int enemyRow = 0;
	int enemyCol = 0;
	int prevEnemyRow = 0;
	int prevEnemyCol = 0;

	player_finder(ROWS, COLS, world, playerRow, playerCol);

	for (int i = 0; i < ROWS; i++) { // Search for enemies and move them to player.
		for (int j = 0; j < COLS; j++) {
			if (world[i][j] == ENEMY) {
				enemyRow = i;
				enemyCol = j;
				prevEnemyRow = i;
				prevEnemyCol = j;
				world[i][j] = TILE;
				if (playerRow < enemyRow) {
					enemyRow--;
				}
				else if (playerRow > enemyRow) {
					enemyRow++;
				}

				if (playerCol < enemyCol) {
					enemyCol--;
				}
				else if (playerCol > enemyCol) {
					enemyCol++;
				}

				path_find(world, ROWS, COLS, enemyRow, enemyCol, prevEnemyRow, prevEnemyCol);
				
				if ((enemyRow == playerRow) && (enemyCol == playerCol)) {
					enemyRow++;
					while (world[enemyRow][enemyCol] == ENEMY) {
						enemyCol++;
					}
				}

				if ((enemyRow > ROWS) || (enemyRow < 0) || (enemyCol > COLS) || (enemyCol < 0)) {
					enemyRow = i;
					enemyCol = j;
				}

				path_find(world, ROWS, COLS, enemyRow, enemyCol, prevEnemyRow, prevEnemyCol);

				world[enemyRow][enemyCol] = ENEMYMOVED;
			}
		}
	}

	output_world(ROWS, COLS, world, healthPoints, hp, MAXHP, level);

	if (fight_check(world, ROWS, COLS, playerRow, playerCol)) { // If player is in range for a fight.
		setColor(12);
		cout << "Enemy attacks." << endl;
		setColor(15);
		cout << "Rolling D6..." << endl;
		Sleep(200);
		srand(time(0));
		int roll_D6 = rand() % 6 + 1; // random number from 1 to 6.
		cout << roll_D6 << endl;

		if (roll_D6 == 6) {
			setColor(212);
			cout << "Critical! Enemy hits for 2 damage." << endl;
			hp -= 2;
			Sleep(420);
		}
		else if (roll_D6 > 3) {
			setColor(12);
			cout << "Enemy hits for 1 damage." << endl;
			hp--;
			Sleep(420);
		}
		else {
			setColor(10);
			cout << "Enemy missed." << endl;
			Sleep(420);
		}
	}
}

void wave(int& level, char world[][WORLD_COLS], const int ROWS, const int COLS, bool& newLevel) { // contains enemy spawning
	int enemyCount = 0; // for checking if there are no enemies

	for (int i = 0; i < ROWS; i++) { // counts the enemies in the world.
		for (int j = 0; j < COLS; j++) {
			if (world[i][j] == ENEMY) {
				enemyCount++;
			}
		}
	}

	if (enemyCount == 0) { // spawning levels
		for (int i = 0; i < (((2 * (ROWS + COLS)) + ((2 * (ROWS + COLS)) - 4)) - 5), i <= level + 1; i++) { // the amount of enemies will be level + 1 until it runs out of spawning space.

			int enemyRow = 0;
			int enemyCol = 0;

			srand(time(0));
			int side = rand() % 4; // 0, top; 1, bottom; 2, left; 3, right.
			int inOrOutPerim = rand() % 2; // 0, outer perimeter; 1, inner perimeter.
			if ((side == 0) || (side == 1))
				enemyCol = rand() % COLS; // for top or bottom side
			else
				enemyRow = rand() % ROWS; // for left or right side

			switch (side) {
			case 0:
				if (inOrOutPerim == 0) {
					enemyRow = 0;
				}
				else {
					enemyRow = 1;
				}
				break;
			case 1:
				if (inOrOutPerim == 0) {
					enemyRow = ROWS - 1;
				}
				else {
					enemyRow = ROWS - 2;
				}
				break;
			case 2:
				if (inOrOutPerim == 0) {
					enemyCol = 0;
				}
				else {
					enemyCol = 1;
				}
				break;
			case 3:
				if (inOrOutPerim == 0) {
					enemyCol = COLS - 1;
				}
				else {
					enemyCol = COLS - 2;
				}
				break;
			}

			while (occupied(world, enemyRow, enemyCol)) { // check for occupied spaces
				side = rand() % 4;
				inOrOutPerim = rand() % 2;
				if ((side == 0) || (side == 1))
					enemyCol = rand() % COLS; // for top or bottom side
				else
					enemyRow = rand() % ROWS; // for left or right side

				switch (side) {
				case 0:
					if (inOrOutPerim == 0) {
						enemyRow = 0;
					}
					else {
						enemyRow = 1;
					}
					break;
				case 1:
					if (inOrOutPerim == 0) {
						enemyRow = ROWS - 1;
					}
					else {
						enemyRow = ROWS - 2;
					}
					break;
				case 2:
					if (inOrOutPerim == 0) {
						enemyCol = 0;
					}
					else {
						enemyCol = 1;
					}
					break;
				case 3:
					if (inOrOutPerim == 0) {
						enemyCol = COLS - 1;
					}
					else {
						enemyCol = COLS - 2;
					}
					break;
				}
			}
			world[enemyRow][enemyCol] = ENEMY; // spawn an enemy
		}

		level++;
		newLevel = true;
	}
}

void powUpSpawner(char world[][WORLD_COLS], const int ROWS, const int COLS, const char POWUP) {	/////// Happy 4/20! 2018
	// generate random row and col
	int powUpRow = rand() % ROWS;
	int powUpCol = rand() % COLS;

	// check if occupied
	while (occupied(world, powUpRow, powUpCol)) {
		powUpRow = rand() % ROWS;
		powUpCol = rand() % COLS;
	}

	world[powUpRow][powUpCol] = POWUP;
}

void gameProcess() {
	bool gameOver = false;
	int level = 0;
	bool isNewLevel = false;

	bool hasPowUp_Expld = false;
	bool hasPowUp_LngSpr = false;
	/// level countdowns
	int expldCountdown = 2;
	int lngSprCountdown = 2;

	//const int X = 7;
	//const int Y = 7;
	//const int Z = 7;
	//const int W = 7; // Spissitude
	//int MobiusWhale[X][Y][Z][W]; // A fourth dimensional whale. "I can see sideways in time!"

	const int ROWS = WORLD_ROWS;
	const int COLS = WORLD_COLS;
	char world[ROWS][COLS];

	int playerRow = 0;
	int playerCol = 0;

	const int MAXHP = 10;
	int hp = 10; // Current actual HP of Player. Should not exceed MAXHP.
	char health[MAXHP]; // for outputing HP
	for (int i = 0; i < MAXHP; i++) {
		health[i] = 254;
	}

	for (int i = 0; i < ROWS; i++) { // Fill world with tiles
		for (int j = 0; j < COLS; j++) {
			world[i][j] = TILE;
		}
	}

	//long long int lol = 7777777;
	int key = 0;
	int stabSpeed = 230;

	world[ROWS / 2][COLS / 2] = PLAYER; // start player in the middle
	player_finder(ROWS, COLS, world, playerRow, playerCol);

	cout << " /$$$$$$$                                         /$$              /$$$$$$$" << endl;
	cout << "| $$__  $$                                       | $/             | $$__  $$" << endl;
	cout << "| $$  \\ $$  /$$$$$$  /$$    /$$ /$$$$$$  /$$$$$$$|_//$$$$$$$      | $$  \\ $$  /$$$$$$   /$$$$$$   /$$$$$$" << endl;
	cout << "| $$$$$$$/ |____  $$|  $$  /$$//$$__  $$| $$__  $$ /$$_____/      | $$  | $$ /$$__  $$ /$$__  $$ /$$__  $$" << endl;
	cout << "| $$__  $$  /$$$$$$$ \\  $$/$$/| $$$$$$$$| $$  \\ $$|  $$$$$$       | $$  | $$| $$$$$$$$| $$$$$$$$| $$  \\ $$" << endl;
	cout << "| $$  \\ $$ /$$__  $$  \\  $$$/ | $$_____/| $$  | $$ \\____  $$      | $$  | $$| $$_____/| $$_____/| $$  | $$" << endl;
	cout << "| $$  | $$|  $$$$$$$   \\  $/  |  $$$$$$$| $$  | $$ /$$$$$$$/      | $$$$$$$/|  $$$$$$$|  $$$$$$$| $$$$$$$/" << endl;
	cout << "|__/  |__/ \\_______/    \\_/    \\_______/|__/  |__/|_______/       |_______/  \\_______/ \\_______/| $$____/" << endl;
	cout << "                                                                                                | $$" << endl;
	cout << "                                                                                                | $$" << endl;
	cout << "                                                                                                |__/" << endl;
	cout << "Version: " << VERSION << endl;
	cout << "Use the arrow keys to move. WASD to spear (in the respective direction)." << endl;
	cout << "Get ready freddy!" << endl;
	system("pause");

	output_world(ROWS, COLS, world, health, hp, MAXHP, level);

	while (!gameOver) {
		isNewLevel = false;

		if (expldCountdown == 0) { // when the power-up timer runs out, disable the power-up and reset the countdown.
			hasPowUp_Expld = false;
			expldCountdown = 2;
		}
		if (lngSprCountdown == 0) {
			hasPowUp_LngSpr = false;
			lngSprCountdown = 2;
		}

		output_world(ROWS, COLS, world, health, hp, MAXHP, level);

		// Player move
		key = 0;
		cout << "Move..." << endl;
		if ((hasPowUp_Expld) && (hasPowUp_LngSpr)) {
			switch ((key = _getch())) {
			case 'w':
			case 's':
			case 'a':
			case 'd':
				if (isInWorldBounds(playerRow - 1, playerCol - 1, ROWS, COLS)) {
					world[playerRow - 1][playerCol - 1] = 219;
				}
				if (isInWorldBounds(playerRow - 1, playerCol, ROWS, COLS)) {
					world[playerRow - 1][playerCol] = 219;
				}
				if (isInWorldBounds(playerRow - 1, playerCol + 1, ROWS, COLS)) {
					world[playerRow - 1][playerCol + 1] = 219;
				}

				if (isInWorldBounds(playerRow, playerCol - 1, ROWS, COLS)) {
					world[playerRow][playerCol - 1] = 219;
				}
				if (isInWorldBounds(playerRow, playerCol + 1, ROWS, COLS)) {
					world[playerRow][playerCol + 1] = 219;
				}

				if (isInWorldBounds(playerRow + 1, playerCol - 1, ROWS, COLS)) {
					world[playerRow + 1][playerCol - 1] = 219;
				}
				if (isInWorldBounds(playerRow + 1, playerCol, ROWS, COLS)) {
					world[playerRow + 1][playerCol] = 219;
				}
				if (isInWorldBounds(playerRow + 1, playerCol + 1, ROWS, COLS)) {
					world[playerRow + 1][playerCol + 1] = 219;
				}

				if (isInWorldBounds(playerRow - 2, playerCol, ROWS, COLS)) {
					world[playerRow - 2][playerCol] = 219;
				}
				if (isInWorldBounds(playerRow + 2, playerCol, ROWS, COLS)) {
					world[playerRow + 2][playerCol] = 219;
				}
				if (isInWorldBounds(playerRow, playerCol - 2, ROWS, COLS)) {
					world[playerRow][playerCol - 2] = 219;
				}
				if (isInWorldBounds(playerRow, playerCol + 2, ROWS, COLS)) {
					world[playerRow][playerCol + 2] = 219;
				}

				if (isInWorldBounds(playerRow - 3, playerCol, ROWS, COLS)) {
					world[playerRow - 3][playerCol] = 219;
				}
				if (isInWorldBounds(playerRow - 2, playerCol + 1, ROWS, COLS)) {
					world[playerRow - 2][playerCol + 1] = 219;
				}
				if (isInWorldBounds(playerRow - 1, playerCol + 2, ROWS, COLS)) {
					world[playerRow - 1][playerCol + 2] = 219;
				}
				if (isInWorldBounds(playerRow, playerCol + 3, ROWS, COLS)) {
					world[playerRow][playerCol + 3] = 219;
				}
				if (isInWorldBounds(playerRow + 1, playerCol + 2, ROWS, COLS)) {
					world[playerRow + 1][playerCol + 2] = 219;
				}
				if (isInWorldBounds(playerRow + 2, playerCol + 1, ROWS, COLS)) {
					world[playerRow + 2][playerCol + 1] = 219;
				}
				if (isInWorldBounds(playerRow + 3, playerCol, ROWS, COLS)) {
					world[playerRow + 3][playerCol] = 219;
				}
				if (isInWorldBounds(playerRow + 2, playerCol - 1, ROWS, COLS)) {
					world[playerRow + 2][playerCol - 1] = 219;
				}
				if (isInWorldBounds(playerRow + 1, playerCol - 2, ROWS, COLS)) {
					world[playerRow + 1][playerCol - 2] = 219;
				}
				if (isInWorldBounds(playerRow, playerCol - 3, ROWS, COLS)) {
					world[playerRow][playerCol - 3] = 219;
				}
				if (isInWorldBounds(playerRow - 1, playerCol - 2, ROWS, COLS)) {
					world[playerRow - 1][playerCol - 2] = 219;
				}
				if (isInWorldBounds(playerRow - 2, playerCol - 1, ROWS, COLS)) {
					world[playerRow - 2][playerCol - 1] = 219;
				}

				output_world(ROWS, COLS, world, health, hp, MAXHP, level);
				Sleep(stabSpeed);

				/// perhaps inefficient; checks again so it can output and sleep before putting things back to normal.
				if (isInWorldBounds(playerRow - 1, playerCol - 1, ROWS, COLS))
					world[playerRow - 1][playerCol - 1] = TILE;
				if (isInWorldBounds(playerRow - 1, playerCol, ROWS, COLS))
					world[playerRow - 1][playerCol] = TILE;
				if (isInWorldBounds(playerRow - 1, playerCol + 1, ROWS, COLS))
					world[playerRow - 1][playerCol + 1] = TILE;

				if (isInWorldBounds(playerRow, playerCol - 1, ROWS, COLS))
					world[playerRow][playerCol - 1] = TILE;
				if (isInWorldBounds(playerRow, playerCol + 1, ROWS, COLS))
					world[playerRow][playerCol + 1] = TILE;

				if (isInWorldBounds(playerRow + 1, playerCol - 1, ROWS, COLS))
					world[playerRow + 1][playerCol - 1] = TILE;
				if (isInWorldBounds(playerRow + 1, playerCol, ROWS, COLS))
					world[playerRow + 1][playerCol] = TILE;
				if (isInWorldBounds(playerRow + 1, playerCol + 1, ROWS, COLS))
					world[playerRow + 1][playerCol + 1] = TILE;

				if (isInWorldBounds(playerRow - 2, playerCol, ROWS, COLS))
					world[playerRow - 2][playerCol] = TILE;
				if (isInWorldBounds(playerRow + 2, playerCol, ROWS, COLS))
					world[playerRow + 2][playerCol] = TILE;
				if (isInWorldBounds(playerRow, playerCol - 2, ROWS, COLS))
					world[playerRow][playerCol - 2] = TILE;
				if (isInWorldBounds(playerRow, playerCol + 2, ROWS, COLS))
					world[playerRow][playerCol + 2] = TILE;

				if (isInWorldBounds(playerRow - 3, playerCol, ROWS, COLS))
					world[playerRow - 3][playerCol] = TILE;
				if (isInWorldBounds(playerRow - 2, playerCol + 1, ROWS, COLS))
					world[playerRow - 2][playerCol + 1] = TILE;
				if (isInWorldBounds(playerRow - 1, playerCol + 2, ROWS, COLS))
					world[playerRow - 1][playerCol + 2] = TILE;
				if (isInWorldBounds(playerRow, playerCol + 3, ROWS, COLS))
					world[playerRow][playerCol + 3] = TILE;
				if (isInWorldBounds(playerRow + 1, playerCol + 2, ROWS, COLS))
					world[playerRow + 1][playerCol + 2] = TILE;
				if (isInWorldBounds(playerRow + 2, playerCol + 1, ROWS, COLS))
					world[playerRow + 2][playerCol + 1] = TILE;
				if (isInWorldBounds(playerRow + 3, playerCol, ROWS, COLS))
					world[playerRow + 3][playerCol] = TILE;
				if (isInWorldBounds(playerRow + 2, playerCol - 1, ROWS, COLS))
					world[playerRow + 2][playerCol - 1] = TILE;
				if (isInWorldBounds(playerRow + 1, playerCol - 2, ROWS, COLS))
					world[playerRow + 1][playerCol - 2] = TILE;
				if (isInWorldBounds(playerRow, playerCol - 3, ROWS, COLS))
					world[playerRow][playerCol - 3] = TILE;
				if (isInWorldBounds(playerRow - 1, playerCol - 2, ROWS, COLS))
					world[playerRow - 1][playerCol - 2] = TILE;
				if (isInWorldBounds(playerRow - 2, playerCol - 1, ROWS, COLS))
					world[playerRow - 2][playerCol - 1] = TILE;
				break;
			case KEY_UP:
				if (playerRow != 0) {
					world[playerRow][playerCol] = TILE;
					playerRow--;
				}
				break;
			case KEY_DOWN:
				if (playerRow != ROWS - 1) {
					world[playerRow][playerCol] = TILE;
					playerRow++;
				}
				break;
			case KEY_LEFT:
				if (playerCol != 0) {
					world[playerRow][playerCol] = TILE;
					playerCol--;
				}
				break;
			case KEY_RIGHT:
				if (playerCol != COLS - 1) {
					world[playerRow][playerCol] = TILE;
					playerCol++;
				}
				break;
			}
		}
		else if (hasPowUp_Expld) {
			switch ((key = _getch())) {
			case 'w':
			case 's':
			case 'a':
			case 'd':
				if (isInWorldBounds(playerRow - 1, playerCol - 1, ROWS, COLS)) {
					world[playerRow - 1][playerCol - 1] = 219;
				}
				if (isInWorldBounds(playerRow - 1, playerCol, ROWS, COLS)) {
					world[playerRow - 1][playerCol] = 219;
				}
				if (isInWorldBounds(playerRow - 1, playerCol + 1, ROWS, COLS)) {
					world[playerRow - 1][playerCol + 1] = 219;
				}

				if (isInWorldBounds(playerRow, playerCol - 1, ROWS, COLS)) {
					world[playerRow][playerCol - 1] = 219;
				}
				if (isInWorldBounds(playerRow, playerCol + 1, ROWS, COLS)) {
					world[playerRow][playerCol + 1] = 219;
				}

				if (isInWorldBounds(playerRow + 1, playerCol - 1, ROWS, COLS)) {
					world[playerRow + 1][playerCol - 1] = 219;
				}
				if (isInWorldBounds(playerRow + 1, playerCol, ROWS, COLS)) {
					world[playerRow + 1][playerCol] = 219;
				}
				if (isInWorldBounds(playerRow + 1, playerCol + 1, ROWS, COLS)) {
					world[playerRow + 1][playerCol + 1] = 219;
				}

				if (isInWorldBounds(playerRow - 2, playerCol, ROWS, COLS)) {
					world[playerRow - 2][playerCol] = '#';
				}
				if (isInWorldBounds(playerRow + 2, playerCol, ROWS, COLS)) {
					world[playerRow + 2][playerCol] = '#';
				}
				if (isInWorldBounds(playerRow, playerCol - 2, ROWS, COLS)) {
					world[playerRow][playerCol - 2] = '#';
				}
				if (isInWorldBounds(playerRow, playerCol + 2, ROWS, COLS)) {
					world[playerRow][playerCol + 2] = '#';
				}

				output_world(ROWS, COLS, world, health, hp, MAXHP, level);
				Sleep(stabSpeed);

				if (isInWorldBounds(playerRow - 1, playerCol - 1, ROWS, COLS))
					world[playerRow - 1][playerCol - 1] = TILE;
				if (isInWorldBounds(playerRow - 1, playerCol, ROWS, COLS))
					world[playerRow - 1][playerCol] = TILE;
				if (isInWorldBounds(playerRow - 1, playerCol + 1, ROWS, COLS))
					world[playerRow - 1][playerCol + 1] = TILE;

				if (isInWorldBounds(playerRow, playerCol - 1, ROWS, COLS))
					world[playerRow][playerCol - 1] = TILE;
				if (isInWorldBounds(playerRow, playerCol + 1, ROWS, COLS))
					world[playerRow][playerCol + 1] = TILE;

				if (isInWorldBounds(playerRow + 1, playerCol - 1, ROWS, COLS))
					world[playerRow + 1][playerCol - 1] = TILE;
				if (isInWorldBounds(playerRow + 1, playerCol, ROWS, COLS))
					world[playerRow + 1][playerCol] = TILE;
				if (isInWorldBounds(playerRow + 1, playerCol + 1, ROWS, COLS))
					world[playerRow + 1][playerCol + 1] = TILE;

				if (isInWorldBounds(playerRow - 2, playerCol, ROWS, COLS))
					world[playerRow - 2][playerCol] = TILE;
				if (isInWorldBounds(playerRow + 2, playerCol, ROWS, COLS))
					world[playerRow + 2][playerCol] = TILE;
				if (isInWorldBounds(playerRow, playerCol - 2, ROWS, COLS))
					world[playerRow][playerCol - 2] = TILE;
				if (isInWorldBounds(playerRow, playerCol + 2, ROWS, COLS))
					world[playerRow][playerCol + 2] = TILE;
				break;
			case KEY_UP:
				if (playerRow != 0) {
					world[playerRow][playerCol] = TILE;
					playerRow--;
				}
				break;
			case KEY_DOWN:
				if (playerRow != ROWS - 1) {
					world[playerRow][playerCol] = TILE;
					playerRow++;
				}
				break;
			case KEY_LEFT:
				if (playerCol != 0) {
					world[playerRow][playerCol] = TILE;
					playerCol--;
				}
				break;
			case KEY_RIGHT:
				if (playerCol != COLS - 1) {
					world[playerRow][playerCol] = TILE;
					playerCol++;
				}
				break;
			}
		}
		else if (hasPowUp_LngSpr) {
			switch ((key = _getch())) {
			case 'w':
				if (isInWorldBounds(playerRow - 1, playerCol, ROWS, COLS)) {
					world[playerRow - 1][playerCol] = 186;
				}
				if (isInWorldBounds(playerRow - 2, playerCol, ROWS, COLS)) {
					world[playerRow - 2][playerCol] = 186;
				}
				if (isInWorldBounds(playerRow - 3, playerCol, ROWS, COLS)) {
					world[playerRow - 3][playerCol] = 186;
				}
				if (isInWorldBounds(playerRow - 4, playerCol, ROWS, COLS)) {
					world[playerRow - 4][playerCol] = '^';
				}

				output_world(ROWS, COLS, world, health, hp, MAXHP, level);
				Sleep(stabSpeed);

				if (isInWorldBounds(playerRow - 1, playerCol, ROWS, COLS))
					world[playerRow - 1][playerCol] = TILE;
				if (isInWorldBounds(playerRow - 2, playerCol, ROWS, COLS))
					world[playerRow - 2][playerCol] = TILE;
				if (isInWorldBounds(playerRow - 3, playerCol, ROWS, COLS))
					world[playerRow - 3][playerCol] = TILE;
				if (isInWorldBounds(playerRow - 4, playerCol, ROWS, COLS))
					world[playerRow - 4][playerCol] = TILE;
				break;
			case 's':
				if (isInWorldBounds(playerRow + 1, playerCol, ROWS, COLS)) {
					world[playerRow + 1][playerCol] = 186;
				}
				if (isInWorldBounds(playerRow + 2, playerCol, ROWS, COLS)) {
					world[playerRow + 2][playerCol] = 186;
				}
				if (isInWorldBounds(playerRow + 3, playerCol, ROWS, COLS)) {
					world[playerRow + 3][playerCol] = 186;
				}
				if (isInWorldBounds(playerRow + 4, playerCol, ROWS, COLS)) {
					world[playerRow + 4][playerCol] = 'V';
				}

				output_world(ROWS, COLS, world, health, hp, MAXHP, level);
				Sleep(stabSpeed);

				world[playerRow + 1][playerCol] = TILE;
				world[playerRow + 2][playerCol] = TILE;
				world[playerRow + 3][playerCol] = TILE;
				world[playerRow + 4][playerCol] = TILE;
				break;
			case 'a':
				if (isInWorldBounds(playerRow, playerCol - 1, ROWS, COLS)) {
					world[playerRow][playerCol - 1] = 205;
				}
				if (isInWorldBounds(playerRow, playerCol - 2, ROWS, COLS)) {
					world[playerRow][playerCol - 2] = 205;
				}
				if (isInWorldBounds(playerRow, playerCol - 3, ROWS, COLS)) {
					world[playerRow][playerCol - 3] = 205;
				}
				if (isInWorldBounds(playerRow, playerCol - 4, ROWS, COLS)) {
					world[playerRow][playerCol - 4] = '<';
				}

				output_world(ROWS, COLS, world, health, hp, MAXHP, level);
				Sleep(stabSpeed);

				world[playerRow][playerCol - 1] = TILE;
				world[playerRow][playerCol - 2] = TILE;
				world[playerRow][playerCol - 3] = TILE;
				world[playerRow][playerCol - 4] = TILE;
				break;
			case 'd':
				if (isInWorldBounds(playerRow, playerCol + 1, ROWS, COLS)) {
					world[playerRow][playerCol + 1] = 205;
				}
				if (isInWorldBounds(playerRow, playerCol + 2, ROWS, COLS)) {
					world[playerRow][playerCol + 2] = 205;
				}
				if (isInWorldBounds(playerRow, playerCol + 3, ROWS, COLS)) {
					world[playerRow][playerCol + 3] = 205;
				}
				if (isInWorldBounds(playerRow, playerCol + 4, ROWS, COLS)) {
					world[playerRow][playerCol + 4] = '>';
				}

				output_world(ROWS, COLS, world, health, hp, MAXHP, level);
				Sleep(stabSpeed);

				world[playerRow][playerCol + 1] = TILE;
				world[playerRow][playerCol + 2] = TILE;
				world[playerRow][playerCol + 3] = TILE;
				world[playerRow][playerCol + 4] = TILE;
				break;
			case KEY_UP:
				if (playerRow != 0) {
					world[playerRow][playerCol] = TILE;
					playerRow--;
				}
				break;
			case KEY_DOWN:
				if (playerRow != ROWS - 1) {
					world[playerRow][playerCol] = TILE;
					playerRow++;
				}
				break;
			case KEY_LEFT:
				if (playerCol != 0) {
					world[playerRow][playerCol] = TILE;
					playerCol--;
				}
				break;
			case KEY_RIGHT:
				if (playerCol != COLS - 1) {
					world[playerRow][playerCol] = TILE;
					playerCol++;
				}
				break;
			}
		}
		else {
			switch ((key = _getch())) {
			case 'w':
				if (playerRow - 1 >= 0) {
					world[playerRow - 1][playerCol] = '|';
					if (playerRow - 2 >= 0) {
						world[playerRow - 2][playerCol] = '^';
						output_world(ROWS, COLS, world, health, hp, MAXHP, level);
						Sleep(stabSpeed);
						world[playerRow - 2][playerCol] = TILE;
					}
					else {
						output_world(ROWS, COLS, world, health, hp, MAXHP, level);
						Sleep(stabSpeed);
					}
					world[playerRow - 1][playerCol] = TILE;
				}
				break;
			case 's':
				if (playerRow + 1 < ROWS) {
					world[playerRow + 1][playerCol] = '|';
					if (playerRow + 2 < ROWS) {
						world[playerRow + 2][playerCol] = 'V';
						output_world(ROWS, COLS, world, health, hp, MAXHP, level);
						Sleep(stabSpeed);
						world[playerRow + 2][playerCol] = TILE;
					}
					else {
						output_world(ROWS, COLS, world, health, hp, MAXHP, level);
						Sleep(stabSpeed);
					}
					world[playerRow + 1][playerCol] = TILE;
				}
				break;
			case 'a':
				if (playerCol - 1 >= 0) {
					world[playerRow][playerCol - 1] = '-';
					if (playerCol - 2 >= 0) {
						world[playerRow][playerCol - 2] = '<';
						output_world(ROWS, COLS, world, health, hp, MAXHP, level);
						Sleep(stabSpeed);
						world[playerRow][playerCol - 2] = TILE;
					}
					else {
						output_world(ROWS, COLS, world, health, hp, MAXHP, level);
						Sleep(stabSpeed);
					}
					world[playerRow][playerCol - 1] = TILE;
				}
				break;
			case 'd':
				if (playerCol + 1 < COLS) {
					world[playerRow][playerCol + 1] = '-';
					if (playerCol + 2 < COLS) {
						world[playerRow][playerCol + 2] = '>';
						output_world(ROWS, COLS, world, health, hp, MAXHP, level);
						Sleep(stabSpeed);
						world[playerRow][playerCol + 2] = TILE;
					}
					else {
						output_world(ROWS, COLS, world, health, hp, MAXHP, level);
						Sleep(stabSpeed);
					}
					world[playerRow][playerCol + 1] = TILE;
				}

				break;
			case KEY_UP:
				if (playerRow != 0) {
					world[playerRow][playerCol] = TILE;
					playerRow--;
				}
				break;
			case KEY_DOWN:
				if (playerRow != ROWS - 1) {
					world[playerRow][playerCol] = TILE;
					playerRow++;
				}
				break;
			case KEY_LEFT:
				if (playerCol != 0) {
					world[playerRow][playerCol] = TILE;
					playerCol--;
				}
				break;
			case KEY_RIGHT:
				if (playerCol != COLS - 1) {
					world[playerRow][playerCol] = TILE;
					playerCol++;
				}
				break;
			}
		}

		if (world[playerRow][playerCol] == POWUP_EXPLD) {
			hasPowUp_Expld = true;
		}
		if (world[playerRow][playerCol] == POWUP_LNGSPR) {
			hasPowUp_LngSpr = true;
		}

		world[playerRow][playerCol] = PLAYER;
		output_world(ROWS, COLS, world, health, hp, MAXHP, level);
		// Player is moved

		enemy_ai(world, ROWS, COLS, hp, health, MAXHP, level);
		reset_moved_enemies(world, ROWS, COLS);

		wave(level, world, ROWS, COLS, isNewLevel);

		// Power-up spawning
		if (isNewLevel) {
			if (hasPowUp_Expld) {
				expldCountdown--;
			}
			if (hasPowUp_LngSpr) {
				lngSprCountdown--;
			}

			int expld_chance = rand() % 18; // Explosion power-up spawn chance; 1 in 18
			int lngSpr_chance = rand() % 8; // Long spear power-up spawn chance; 1 in 8

											 // Power-up spawn conditions
			if (expld_chance == 7) {
				powUpSpawner(world, ROWS, COLS, POWUP_EXPLD);
			}
			if (lngSpr_chance == 7) {
				powUpSpawner(world, ROWS, COLS, POWUP_LNGSPR);
			}
		}

		// Game over condition
		if (hp <= 0) {
			system("CLS"); // Clear screen
			setColor(12);
			cout << "\n\n\n       Game Over\n\n\n" << endl;
			cout << "\nYou survived " << level - 1 << " level(s)" << endl;

			ifstream highScores_In;
			highScores_In.open("High Scores.txt");
			// Check for error
			if (highScores_In.fail())
				cerr << "Error opening high score file." << endl;

			int score = 0;

			highScores_In >> score; /// Only works half the time. ¯\_(ツ)_/¯
			highScores_In.close();

			ofstream highScores_Out;
			highScores_Out.open("High Scores.txt");

			if (highScores_Out.is_open()) {

				if (level - 1 > score) {
					highScores_Out << level - 1;
					cout << "New high score!" << endl;
					cout << "High score: " << level - 1;
				}
				else {
					highScores_Out << score;
					cout << "High score: " << score;
				}

				highScores_Out.close();
			}
			else {
				cerr << "\nUnable to open high score file." << endl;
			}

			gameOver = true;
		}
	}
}

int main() {
	/*using namespace graphics;

	Window window("Raven's Deep", 1280, 720);

	while (!window.closed()) {
		window.update();
	}*/

	gameProcess();
	
	return 0;
}