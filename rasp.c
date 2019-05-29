#include <stdio.h>
#include <wiringPi.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include "raspLib.h"

#define MENU 0
#define CATCH_ME 1
#define FILLING_MORE 2
#define ROULETTE 3

#define GAMES_NUMBER 3

void rightButtonned();
void leftButtonned();

void changeGamemode(int gamemode);
void beforeGame();
void afterGame();
void displayPoints();

//Corps

int resetCounter = 0;//Handle the reset double buttons

int interuptHandling = 0;//Multi-threading handling by stopping core program

int generalState = -1;
int gameState = 0;
int playerCursor = 0; //Gestion du curseur du joueur (menu principal / roulette / etc...)
int sens = 0;

int pointPlayer1 = 0;
int pointPlayer2 = 0;

int rouletteSpeed = 0;
int gameCursor = 0;//Gestion du curseur du jeu (Curseur roulette, curseur filling_more)


int main(void)
{
    wiringPiSetup();
    setupBargraph();
    setupLeds();
    setupButtons();
	wiringPiISR(BUTTON_RIGHT, INT_EDGE_FALLING, &rightButtonned);
	wiringPiISR(BUTTON_LEFT, INT_EDGE_FALLING, &leftButtonned);

    changeGamemode(MENU);

	while(TRUE){

		if(readRightButton() == 1 && readLeftButton() == 1){
			resetCounter++;
			//Si les deux boutons sont appuyés pendant 10 boucles (dépendant du jeu)
			if(resetCounter > 10){
				printf("#########\n");
				printf("# RESET #\n");
				printf("#########\n\n\n\n\n");
    			changeGamemode(MENU);//Retour menu
			}
		} else {
			resetCounter = 0;
		}

		while(interuptHandling == 1){}//Gestion anti-multithreading des interuptButtons

		switch(generalState) {
			case MENU :
				toggleGreenLed();
				writeRedLed(0);
				writeBargraphUniLed(playerCursor+1);
				delay(200);
				break;

			case CATCH_ME :
				if(sens == 0){
					playerCursor = playerCursor + 1;
				} else {
					playerCursor = playerCursor - 1;
				}
				if(playerCursor >= BG_MAX-1){
					sens = 1;
				} else if(playerCursor <= 0) {
					sens = 0;
				}
				writeBargraphUniLed(playerCursor+1);

				delay(60);
				break;

			case FILLING_MORE :
				if(sens == 0){
					playerCursor = playerCursor + 1;
				} else {
					playerCursor = playerCursor - 1;
				}
				if(playerCursor >= gameCursor-1){
					sens = 1;
				} else if(playerCursor <= 0) {
					sens = 0;
				}

				//Affiche le curseur du jeu
				int total = 0;
				if(playerCursor >= 0){
					total += 1;
				}
				if(playerCursor >= 1){
					total += 2;
				}
				if(playerCursor >= 2){
					total += 4;
				}
				if(playerCursor >= 3){
					total += 8;
				}
				if(playerCursor >= 4){
					total += 16;
				}
				if(playerCursor >= 5){
					total += 32;
				}
				if(playerCursor >= 6){
					total += 64;
				}
				if(playerCursor >= 7){
					total += 128;
				}
				writeBargraph(total);


				delay(60);
				break;


			case ROULETTE ://Boucle du jeu Roulette
				if(gameState == 0){//Etat de choix du curseur
					writeBargraphUniLed(playerCursor+1);
				} else if(gameState == 1){//Etat de roulette lancée
					gameCursor++;
					if(gameCursor > 7){
						gameCursor = 0;
					}
					rouletteSpeed = rouletteSpeed * 1.3;

					//Si curseur roulette au même endroit, clignottement
					if(gameCursor == playerCursor){
						writeBargraph(pow(2, gameCursor));
					} else { // Sinon affichage des deux curseurs
						writeBargraph(pow(2, gameCursor) + pow(2, playerCursor));
					}

					if(rouletteSpeed > rand() % 400 + 400){ //Fin de la roulette aléatoire dès que l'attente est grande
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
	playerCursor = 0;
	gameCursor = 0;
	sens = 0;
	pointPlayer1 = 0;
	pointPlayer2 = 0;

	switch(generalState) {
		case MENU :
			printf("MENU PRINCIPAL V1 !\n");
			writeBargraphUniLed(playerCursor);
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
			printf("Bienvenue dans FILLING MORE !\n");
			printf("Mode de jeu : 1 joueur\n");
			printf("Nombre de point à obtenir : 8 points (Remplir la barre)\n");
			printf("Explication: ...\n");
			beforeGame();
			break;

		case ROULETTE :
			printf("Bienvenue dans ROULETTE !\n");
			printf("Mode de jeu : 1 joueur\n");
			printf("Nombre de point à obtenir : 4points\n");
			printf("Explication: ...\n");
			beforeGame();
			break;

		default : 
			printf("erreur interne du logiciel #10\n");

	}
}

void beforeGame(){
	blankBargraph();
	writeGreenLed(0);
	writeRedLed(0);

	switch(generalState) {
		case MENU :
			break;

		case CATCH_ME :
				gameState = 0;
				playerCursor = 4;
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
			gameCursor = 8;//Initial curseur du jeu
			break;

		case ROULETTE :
			rouletteSpeed = 20.0;
			playerCursor = 0;
			gameCursor = 0;
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
				if(playerCursor == 1){
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

		case ROULETTE :
			if(playerCursor == gameCursor){
				for(int i = 0; i < 3; i++){
					writeBargraphUniLed(gameCursor+1);
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
			playerCursor++;
			if (playerCursor > GAMES_NUMBER-1){ 
				playerCursor = 0;
			}
			break;

		case CATCH_ME :
			printf("Bouton appuyé par joueur 1 !\n");
			if(playerCursor == 7){
				pointPlayer1++;
				playerCursor = 1;
			} else {
				playerCursor = 0;
			}
			afterGame();
			break;

		case FILLING_MORE :
			if(playerCursor + 1 == gameCursor){
				writeGreenLed(1);
				for(int i = 0; i < 6; i++){
					blankBargraph();
					delay(200);
					fullBargraph();
					delay(200);
				}
				changeGamemode(MENU);
			} else {
				writeRedLed(1);
				delay(2000);
				writeRedLed(0);
			}
			break;

		case ROULETTE:
			if(gameState == 0){
				playerCursor++;
				if (playerCursor > 7){ 
					playerCursor = 0;
				}
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
			changeGamemode(playerCursor+1);
			break;
			
		case CATCH_ME :
			printf("Bouton appuyé par joueur 2 !\n");
			if(playerCursor == 0){
				pointPlayer2++;
				playerCursor = 1;
			} else {
				playerCursor = 0;
			}
			afterGame();
			break;

		case FILLING_MORE :
			break;

		case ROULETTE:
			if(gameState == 0){
				printf("Lancement de la roulette !\n");
				gameState = 1;
			}
			break;

		default : 
			printf("erreur interne du logiciel #2\n");
	}

	interuptHandling = 0;
}
