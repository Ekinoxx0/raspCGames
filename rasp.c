#include <stdio.h>
#include <wiringPi.h>
#include <math.h>
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

const int pinBargraph[BG_MAX]={BG_1, BG_2,  BG_3,  BG_4,  BG_5,  BG_6,  BG_7,  BG_8};

#define MENU 0
#define CATCH_ME 1
#define FILLING_MORE 2
#define SIMON 3
#define ROULETTE 4

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
int readLeftButton();
int readRightButton();

void rightButtonned();
void leftButtonned();

double pow(double x, double y);
float powf(float x, float y);
long double powl(long double x, long double y);

//Corps

int generalState = -1;
int gameState = 0;
int gameCursor = 0; //Affichage le jeu sélectionner dans le menu principal

int main(void)
{
    wiringPiSetup();
    setupBargraph();
    setupLeds();
    setupButtons();

    changeGamemode(MENU);

	while(TRUE){

		switch(generalState) {
			case MENU :
				toggleGreenLed();
				delay(200);
				break;

			case CATCH_ME :
				break;

			case FILLING_MORE :
				break;

			default : 
				printf("erreur interne du logiciel");
				toggleRedLed();
				writeGreenLed(0);
				blankBargraph();
				delay(100);
		}

	}

    return 0;
}

//
// GAME RELATIVE
//

void changeGamemode(int gamemode){
	switch(generalState) {
		case MENU :
			writeBargraphUniLed(gameCursor);
			writeGreenLed(1);
			writeRedLed(1);
			delay(1000);
			break;

		case CATCH_ME :
			break;

		case FILLING_MORE :
			break;

		default : 
			printf("erreur interne du logiciel");

	}
}


//
// Get Buttonned
//

void rightButtonned(){

	switch(generalState) {
		case MENU :
			gameCursor++;
			if (gameCursor > 1){ 
				gameCursor = 0;
			}

			writeBargraphUniLed(gameCursor+1);

			break;

		case CATCH_ME :
			break;

		case FILLING_MORE :
			break;

		default : 
			printf("erreur interne du logiciel");
	}

}

void leftButtonned(){

	switch(generalState) {
		case MENU :
			changeGamemode(gameCursor+1);
			break;
			
		case CATCH_ME :
			break;

		case FILLING_MORE :
			break;

		default : 
			printf("erreur interne du logiciel");
	}
}


//
// SETUPS
//

void setupBargraph(){
	for (int i = 0; i < BG_MAX; i++) {
        	pinMode(pinBargraph[i], OUTPUT);
	}
	blankBargraph();
}

void setupLeds(){
	pinMode(LED_GREEN, OUTPUT);
	pinMode(LED_RED, OUTPUT);
	writeRedLed(0);
	writeGreenLed(0);
}
void setupButtons(){
	pinMode(BUTTON_RIGHT, INPUT);
	pinMode(BUTTON_LEFT, INPUT);
	wiringPiISR(BUTTON_RIGHT, INT_EDGE_FALLING, &rightButtonned);
	wiringPiISR(BUTTON_LEFT, INT_EDGE_FALLING, &leftButtonned);
}

//
//RELATIVES
//

void writeBargraph(unsigned char stateLedsBG){
	for (int i = 0; i < BG_MAX; i++) {
		int filtre = pow(2,i); 
        	digitalWrite(pinBargraph[i], stateLedsBG & filtre);
	}
}

void writeBargraphUniLed(int uniLed){
	blankBargraph();
	writeBargraph(powl(2, uniLed-1));
}

void blankBargraph(){
	writeBargraph(0b00000000);
}

int IS_GREEN = 0;
void writeGreenLed(int state){
	IS_GREEN = state;
	digitalWrite(LED_GREEN, state);
}

int IS_RED = 0;
void writeRedLed(int state){
	IS_RED = state;
	digitalWrite(LED_RED, state);
}

void toggleGreenLed(){
	writeGreenLed(IS_GREEN == 0 ? 1 : 0);
}

void toggleRedLed(){
	writeRedLed(IS_RED == 0 ? 1 : 0);
}

int readLeftButton(){
	return digitalRead(BUTTON_LEFT) == 0 ? 1 : 0;
}

int readRightButton(){
	return digitalRead(BUTTON_RIGHT) == 0 ? 1 : 0;
}


