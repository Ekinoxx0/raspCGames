#include <stdio.h>
#include <wiringPi.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>

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

#define GAMES_NUMBER 4

void setupBargraph();
void setupLeds();
void setupButtons();

void writeGreenLed(int state);
void writeRedLed(int state);
void toggleGreenLed();
void toggleRedLed();
void writeBargraph(unsigned char stateLedsBG);
void writeBargraphLed(int led);
void writeBargraphUniLed(int led);
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
void displayPoints();

//Corps

int interuptHandling = 0;//Multi-threading handling by stopping core program

int generalState = -1;
int gameState = 0;
int gameCursor = 0; //Affichage le jeu sélectionner dans le menu principal
int sens = 0;

int pointPlayer1 = 0;
int pointPlayer2 = 0;

int rouletteSpeed = 0;
int rouletteCursor = 0;


int main(void)
{
    wiringPiSetup();
    setupBargraph();
    setupLeds();
    setupButtons();

    changeGamemode(MENU);

	while(TRUE){

		while(interuptHandling == 1){}

		switch(generalState) {
			case MENU :
				toggleGreenLed();
				writeRedLed(0);
				writeBargraphUniLed(gameCursor+1);
				delay(200);
				break;

			case CATCH_ME :
				if(sens == 0){
					gameCursor = gameCursor + 1;
				} else {
					gameCursor = gameCursor - 1;
				}
				if(gameCursor >= BG_MAX-1){
					sens = 1;
				} else if(gameCursor <= 0) {
					sens = 0;
				}
				writeBargraphUniLed(gameCursor+1);

				delay(60);
				break;

			case FILLING_MORE :
				break;


			case SIMON :
				break;


			case ROULETTE :
				if(gameState == 0){
					writeBargraphUniLed(gameCursor+1);
				} else if(gameState == 1){
					rouletteCursor++;
					if(rouletteCursor > 7){
						rouletteCursor = 0;
					}
					rouletteSpeed *= 1.1;

					blankBargraph();
					writeBargraphLed(gameCursor+1);
					writeBargraphLed(rouletteCursor+1);

					if(rouletteSpeed > 600){
						afterGame();
						continue;
					}

					delay(rouletteSpeed);
				}
				break;


			default : 
				printf("erreur interne du logiciel #11\n");
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
	printf("Changement de mode de jeu de %d pour %d \n", generalState, gamemode);
	generalState = gamemode;
	gameState = 0;
	gameCursor = 0;
	sens = 0;
	pointPlayer1 = 0;
	pointPlayer2 = 0;

	switch(generalState) {
		case MENU :
			printf("MENU PRINCIPAL !\n");
			writeBargraphUniLed(gameCursor);
			writeGreenLed(1);
			writeRedLed(1);
			delay(1000);
			break;

		case CATCH_ME :
			printf("Bienvenue dans CATCH ME !\n");
			printf("Mode de jeu : 2 joueurs\n");
			printf("Nombre de point à obtenir : 4points\n");
			printf("Explication: Chaque joueur prend un bouton,\n");
			printf("             il doit appuyer sur le bouton quand le lumière est de son coté,\n");
			printf("             si il réussi, la led vert s'allume\n");
			printf("             si il échoue, la led rouge s'allume.\n");
			printf("             à la fin de chaque manche le nombre de point de chaque joueur s'affiche\n");
			beforeGame();
			break;

		case FILLING_MORE :
			printf("Bienvenue dans CATCH ME !\n");
			printf("JEU PAS FINI\n");
			beforeGame();
			break;

		case SIMON :
			printf("Bienvenue dans SIMON!\n");
			printf("JEU PAS FINI\n");
			beforeGame();
			break;

		case ROULETTE :
			printf("Bienvenue dans ROULETTE !\n");
			printf("Mode de jeu : 1 joueur\n");
			beforeGame();
			break;

		default : 
			printf("erreur interne du logiciel #10\n");

	}
}

void beforeGame(){
	writeGreenLed(0);
	writeRedLed(0);

	switch(generalState) {
		case MENU :
			break;

		case CATCH_ME :
				gameState = 0;
				gameCursor = 4;
				sens = rand() % 2;
				fullBargraph();
				delay(200);
				writeBargraph(0b11111100);
				delay(500);
				writeBargraph(0b11110000);
				delay(500);
				writeBargraph(0b11000000);
				delay(500);
			break;

		case FILLING_MORE :
			delay(3000);
			changeGamemode(MENU);
			break;

		case SIMON :
			delay(3000);
			changeGamemode(MENU);
			break;

		case ROULETTE :
			rouletteSpeed = 20;
			gameCursor = 0;
			rouletteCursor = 0;
			printf("Sélectionnez votre case...\n");
			break;

		default : 
			printf("erreur interne du logiciel #20\n");

	}
}

void afterGame(){
	switch(generalState) {
		case MENU :
			break;

		case CATCH_ME :
				if(gameCursor == 1){
					printf("Bien joué !\n");
					writeGreenLed(1);
					writeRedLed(0);
				} else {
					printf("Raté\n");
					writeGreenLed(0);
					writeRedLed(1);
				}
				sleep(2);

				displayPoints();

				if(pointPlayer1 > 3 || pointPlayer2 > 3){
					changeGamemode(MENU);
					return;
				}
			break;

		case FILLING_MORE :
			break;

		case SIMON :
			break;

		case ROULETTE :
			if(gameCursor == rouletteCursor){
				for(int i = 0; i < 3; i++){
					writeBargraphLed(rouletteCursor+1);
					blankBargraph();
					delay(300);
				}
				printf("Bien joué !\n");
				writeGreenLed(1);
				writeRedLed(0);
				pointPlayer1++;
			} else {
				printf("Raté\n");
				writeGreenLed(0);
				writeRedLed(1);
			}
			sleep(2);

			displayPoints();

			if(pointPlayer1 > 3){
				changeGamemode(MENU);
			}
			gameState = 0;
			break;

		default : 
			printf("erreur interne du logiciel #21\n");

	}

	beforeGame();
}

void displayPoints(){
	writeGreenLed(0);
	writeRedLed(0);
	printf("pointPlayer1 : %d\n", pointPlayer1);
	printf("pointPlayer2 : %d\n", pointPlayer2);

	int total = 0;

	if(pointPlayer2 >= 1){
		total += 1;
	}
	if(pointPlayer2 >= 2){
		total += 2;
	}
	if(pointPlayer2 >= 3){
		total += 4;
	}
	if(pointPlayer2 >= 4){
		for(int i = 0; i < 4; i++){
			blankBargraph();
			delay(300);
			writeBargraph(0b00001111);
			delay(300);
		}
		delay(1000);
		return;
	}

	if(pointPlayer1 >= 1){
		total += 128;
	}
	if(pointPlayer1 >= 2){
		total += 64;
	}
	if(pointPlayer1 >= 3){
		total += 32;
	}
	if(pointPlayer1 >= 4){
		for(int i = 0; i < 4; i++){
			blankBargraph();
			delay(300);
			writeBargraph(0b11110000);
			delay(300);
		}
		delay(1000);
		return;
	}

	writeBargraph(total);

	delay(1500);
}


//
// Get Buttonned
//

void rightButtonned(){
	if(interuptHandling == 1){
		return;
	}
	interuptHandling = 1;

	switch(generalState) {
		case MENU :
			printf("Sélection mode de jeu suivant, cliquez sur l'autre bouton pour confirmer.\n");
			gameCursor++;
			if (gameCursor > GAMES_NUMBER-1){ 
				gameCursor = 0;
			}
			break;

		case CATCH_ME :
			printf("Bouton appuyé par joueur 1 !\n");
			if(gameCursor == 7){
				pointPlayer1++;
				gameCursor = 1;
			} else {
				gameCursor = 0;
			}
			afterGame();
			break;

		case FILLING_MORE :
			break;

		case SIMON:
			break;

		case ROULETTE:
			gameCursor++;
			if (gameCursor > 7){ 
				gameCursor = 0;
			}
			break;

		default : 
			printf("erreur interne du logiciel #1\n");
	}

	interuptHandling = 0;
}

void leftButtonned(){
	if(interuptHandling == 1){
		return;
	}
	interuptHandling = 1;

	switch(generalState) {
		case MENU :
			changeGamemode(gameCursor+1);
			break;
			
		case CATCH_ME :
			printf("Bouton appuyé par joueur 2 !\n");
			if(gameCursor == 0){
				pointPlayer2++;
				gameCursor = 1;
			} else {
				gameCursor = 0;
			}
			afterGame();
			break;

		case FILLING_MORE :
			break;

		case SIMON:
			break;

		case ROULETTE:
			printf("Lancement de la roulette !\n");
			gameState = 1;
			break;

		default : 
			printf("erreur interne du logiciel #2\n");
	}

	interuptHandling = 0;
}

void rightButtonnedUp(){

}

void leftButtonnedUp(){
	
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

void writeBargraphLed(int uniLed){
	writeBargraph(powl(2, uniLed-1));
}

void writeBargraphUniLed(int uniLed){
	blankBargraph();
	writeBargraph(powl(2, uniLed-1));
}

void blankBargraph(){
	writeBargraph(0b00000000);
}

void fullBargraph(){
	writeBargraph(0b11111111);
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
