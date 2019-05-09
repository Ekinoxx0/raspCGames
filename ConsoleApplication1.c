#include <stdio.h>
#include <math.h>

#include "wiringPi.h"

#define BG_1 7
#define BG_2 0
#define BG_3 1
#define BG_4 2
#define BG_5 3
#define BG_6 4
#define BG_7 5
#define BG_8 6
#define BG_MAX 8

#define LED_GREEN 26
#define LED_RED 22

#define BUTTON_LEFT 21
#define BUTTON_RIGHT 29

const int pinBargraph[BG_MAX] = { BG_1, BG_2,  BG_3,  BG_4,  BG_5,  BG_6,  BG_7,  BG_8 };

#define MENU 0
#define CATCH_ME 1
#define FILLING_MORE 2
#define SIMON 3
#define ROULETTE 4

#define GAMES_NUMBER 4

void setupBargraph();
void setupLeds();
void setupButtons();

void writeGreenLed(int state);
void writeRedLed(int state);
void toggleGreenLed();
void toggleRedLed();
void writeBargraph(unsigned char stateLedsBG);
void writeBargraphUniLed();
void blankBargraph();
void fullBargraph();
int readLeftButton();
int readRightButton();

void rightButtonned();
void leftButtonned();

double pow(double x, double y);
float powf(float x, float y);
long double powl(long double x, long double y);

void changeGamemode(int gamemode);
void beforeGame();
void afterGame();

//Corps

int interuptHandling = 0;//Multi-threading handling by stopping core program

int generalState = -1;
int gameState = 0;
int gameCursor = 0; //Affichage le jeu sélectionner dans le menu principal
int sens = 0;

int pointPlayer1 = 0;
int pointPlayer2 = 0;


int main(void)
{

	wiringPiSetup();
	setupBargraph();
	setupLeds();
	setupButtons();

	changeGamemode(MENU);

	while (TRUE) {

		while (interuptHandling == 1) {}

		switch (generalState) {
		case MENU:
			toggleGreenLed();
			writeRedLed(0);
			writeBargraphUniLed(gameCursor + 1);
			Sleep(200);
			break;

		case CATCH_ME:
			if (sens == 0) {
				gameCursor = gameCursor + 1;
			}
			else {
				gameCursor = gameCursor - 1;
			}
			if (gameCursor >= BG_MAX - 1) {
				sens = 1;
			}
			else if (gameCursor <= 0) {
				sens = 0;
			}
			writeBargraphUniLed(gameCursor + 1);

			Sleep(60);
			break;

		case FILLING_MORE:
			break;


		case SIMON:
			break;


		case ROULETTE:
			break;


		default:
			printf("erreur interne du logiciel #11\n");
			toggleRedLed();
			writeGreenLed(0);
			blankBargraph();
			Sleep(100);
		}

	}

	return 0;
}

//
// GAME RELATIVE
//

void changeGamemode(int gamemode) {
	printf("Changement de mode de jeu de %d pour %d \n", generalState, gamemode);
	generalState = gamemode;

	switch (generalState) {
	case MENU:
		writeBargraphUniLed(gameCursor);
		writeGreenLed(1);
		writeRedLed(1);
		Sleep(1000);
		break;

	case CATCH_ME:
		gameState = 0;
		gameCursor = 5;
		fullBargraph();
		Sleep(1000);
		break;

	case FILLING_MORE:
		writeRedLed(1);
		writeGreenLed(0);
		blankBargraph();
		break;

	default:
		printf("erreur interne du logiciel #10\n");

	}
}

void beforeGame() {
	switch (generalState) {
	case MENU:
		break;

	case CATCH_ME:
		fullBargraph();
		Sleep(200);
		writeBargraph(0b11111100);
		Sleep(500);
		writeBargraph(0b11110000);
		Sleep(500);
		writeBargraph(0b11000000);
		Sleep(500);
		break;

	case FILLING_MORE:
		break;

	default:
		printf("erreur interne du logiciel #20\n");

	}
}

void afterGame() {
	switch (generalState) {
	case MENU:
		break;

	case CATCH_ME:
		printf("afterGame called by interupt\n");
		fullBargraph();
		Sleep(200);
		if (gameCursor == 1) {
			writeGreenLed(1);
			writeRedLed(0);
		}
		else {
			writeGreenLed(0);
			writeRedLed(1);
		}
		Sleep(2000);
		writeGreenLed(0);
		writeRedLed(0);
		printf("pointPlayer1 : %d\n", pointPlayer1);
		printf("pointPlayer2 : %d\n", pointPlayer1);
		int total = 0;
		writeBargraph(0b00000000);
		Sleep(1500);
		printf("afterGame ended\n");
		break;

	case FILLING_MORE:
		break;

	default:
		printf("erreur interne du logiciel #21\n");

	}

	beforeGame();
}


//
// Get Buttonned
//

void rightButtonned() {
	printf("rightButtonned\n");
	interuptHandling = 1;

	switch (generalState) {
	case MENU:
		gameCursor++;
		if (gameCursor > GAMES_NUMBER - 1) {
			gameCursor = 0;
		}
		break;

	case CATCH_ME:
		printf("RIGHT : %d / %d\n", gameCursor, sens);
		if (gameCursor == 7) {
			gameCursor = 1;
		}
		else {
			gameCursor = 0;
		}
		afterGame();
		break;

	case FILLING_MORE:
		break;

	default:
		printf("erreur interne du logiciel #1\n");
	}

	interuptHandling = 0;
}

void leftButtonned() {
	printf("leftButtonned\n");
	interuptHandling = 1;

	switch (generalState) {
	case MENU:
		changeGamemode(gameCursor + 1);
		break;

	case CATCH_ME:
		printf("LEFT : %d / %d\n", gameCursor, sens);
		if (gameCursor == GAMES_NUMBER - 1) {
			gameCursor = 1;
		}
		else {
			gameCursor = 0;
		}
		afterGame();
		break;

	case FILLING_MORE:
		break;

	default:
		printf("erreur interne du logiciel #2\n");
	}

	interuptHandling = 0;
}


//
// SETUPS
//

void setupBargraph() {
	for (int i = 0; i < BG_MAX; i++) {
		pinMode(pinBargraph[i], OUTPUT);
	}
	blankBargraph();
}

void setupLeds() {
	pinMode(LED_GREEN, OUTPUT);
	pinMode(LED_RED, OUTPUT);
	writeRedLed(0);
	writeGreenLed(0);
}
void setupButtons() {
	pinMode(BUTTON_RIGHT, INPUT);
	pinMode(BUTTON_LEFT, INPUT);
	wiringPiISR(BUTTON_RIGHT, INT_EDGE_FALLING, &rightButtonned);
	wiringPiISR(BUTTON_LEFT, INT_EDGE_FALLING, &leftButtonned);
}

//
//RELATIVES
//

void writeBargraph(unsigned char stateLedsBG) {
	for (int i = 0; i < BG_MAX; i++) {
		int filtre = pow(2, i);
		digitalWrite(pinBargraph[i], stateLedsBG & filtre);
	}
}

void writeBargraphUniLed(int uniLed) {
	blankBargraph();
	writeBargraph(powl(2, uniLed - 1));
}

void blankBargraph() {
	writeBargraph(0b00000000);
}

void fullBargraph() {
	writeBargraph(0b11111111);
}

int IS_GREEN = 0;
void writeGreenLed(int state) {
	IS_GREEN = state;
	digitalWrite(LED_GREEN, state);
}

int IS_RED = 0;
void writeRedLed(int state) {
	IS_RED = state;
	digitalWrite(LED_RED, state);
}

void toggleGreenLed() {
	writeGreenLed(IS_GREEN == 0 ? 1 : 0);
}

void toggleRedLed() {
	writeRedLed(IS_RED == 0 ? 1 : 0);
}

int readLeftButton() {
	return digitalRead(BUTTON_LEFT) == 0 ? 1 : 0;
}

int readRightButton() {
	return digitalRead(BUTTON_RIGHT) == 0 ? 1 : 0;
}

