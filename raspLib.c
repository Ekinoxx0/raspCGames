#include <stdio.h>
#include <wiringPi.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include "raspLib.h"

const int pinBargraph[BG_MAX]={BG_1, BG_2,  BG_3,  BG_4,  BG_5,  BG_6,  BG_7,  BG_8};

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
