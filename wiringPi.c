#include <windows.h>
#include "wiringPi.h"
#include <stdio.h>
#include <process.h> 

const int X_pinBargraph[X_BG_MAX] = { X_BG_1, X_BG_2,  X_BG_3,  X_BG_4,  X_BG_5,  X_BG_6,  X_BG_7,  X_BG_8 };

int setuped = FALSE;
int output_pin[50];
int input_pin[50];

int changeOccured = 0;

int output_pin_state[50];
int input_pin_state[50];

void (*button_functions_edge_falling[2])(void);
void (*button_functions_edge_rising[2])(void);
void (*button_functions_edge_both[2])(void);

void color(int t)
{
	HANDLE H = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(H, 0 * 16 + t);
}

void buttonClick(int buttonPin, int cursor, int sleep) {
	printf("Button click %d", buttonPin);
	input_pin_state[buttonPin] = 1;
		//(*button_functions_edge_rising[cursor])();
		//button_functions_edge_both[cursor]();
	Sleep(sleep);
	input_pin_state[buttonPin] = 0;
		//(*button_functions_edge_falling[cursor])();
		//button_functions_edge_both[cursor]();
}

void* buttonLoop()
{

	while (1 == 1) {
		int button;
		scanf_s("%d", &button);
		if (button == 1) {
			buttonClick(X_BUTTON_LEFT, 0, 100);
		}
		else if (button == 2) {
			buttonClick(X_BUTTON_RIGHT, 1, 100);
		}
		else {
			printf("Unknown button %d", button);
			exit(5);
		}
	}
}

void* displayLoop()
{
	while (1 == 1) {
		if (changeOccured == 0) {}
		changeOccured = 0;

		printf("\n\n");

		color(9);
		printf("          Bargraph     ");

		for (int i = 0; i < X_BG_MAX; i++)
		{
			if (output_pin_state[X_pinBargraph[i]] == 1) {
				color(14);
				printf("+");
			}
			else {
				color(15);
				printf("|");
			}
		}
		color(15);
		printf("\n");

		printf("          LED            ");

		if (output_pin_state[X_LED_GREEN] == 1) {
			color(2);
			printf("+");
		}
		else {
			color(15);
			printf("-");

		}
		printf("  ");

		if (output_pin_state[X_LED_RED] == 1) {
			color(12);
			printf("+");
		}
		else {
			color(15);
			printf("-");
		}

		printf("\n");
		printf("      \n");
		Sleep(300);
	}
	return NULL;
}

int wiringPiSetup(void){
	if (setuped == TRUE) {
		printf("Tried to setup WiringPi twice !\n");
		exit(5);
	}
	else {
		setuped = TRUE;
		printf("#############################################\n");
		printf("#    Welcome to Jocelyn Raspbian Emulator ! #\n");
		printf("#  Press 1 or 2 followed by Enter to click  #\n");
		printf("#    on any virtual button at any moment    #\n");
		printf("#############################################\n");
		Sleep(5000);
		unsigned threadID1;
		_beginthreadex(NULL, 0, &displayLoop, NULL, 0, &threadID1);
		unsigned threadID2;
		_beginthreadex(NULL, 0, &buttonLoop, NULL, 0, &threadID2);
		return TRUE;
	}
	return FALSE;
}

void pinMode(int pin, int mode) {
	switch (mode) {
	case OUTPUT:

		switch (pin) {

		case X_BG_1:
		case X_BG_2:
		case X_BG_3:
		case X_BG_4:
		case X_BG_5:
		case X_BG_6:
		case X_BG_7:
		case X_BG_8:
		case X_BG_MAX:

		case X_LED_GREEN:
		case X_LED_RED:
			if (output_pin[pin] == 1) {
				printf("Already setuped output_pin %d !\n", pin);
				exit(7);
			}
			else {
				output_pin[pin] = 1;
			}
			break;

		default:
			printf("Tried to setup a pin in output mode but needs to be in input mode : %d!\n", pin);
			exit(7);
			break;
		}

		break;

	case INPUT:

		switch (pin) {

		case X_BUTTON_LEFT:
		case X_BUTTON_RIGHT:
			if (input_pin[pin] == 1) {
				printf("Already setuped input_pin %d !\n", pin);
				exit(8);
			}
			else {
				input_pin[pin] = 1;
			}
			break;

		default:
			printf("Tried to setup a pin in output mode but needs to be in output mode : %d!\n", pin);
			exit(7);
			break;
		}
		break;

	default:
		printf("Tried to setup pinMode in unkown mode !\n");
		exit(6);
		break;
	}

}

int wiringPiISR(int pin, int mode, void (*function)(void)) {
	switch (mode) {
	case INT_EDGE_BOTH:
		button_functions_edge_both[pin] = function;
		break;
	case INT_EDGE_RISING:
		button_functions_edge_rising[pin] = function;
		break;
	case INT_EDGE_FALLING:
		button_functions_edge_falling[pin] = function;
		break;

	default:
		printf("Setup with unknown mode %d", mode);
		exit(10);
		break;
	}
	return 1;
}

int  digitalRead(int pin) {
	return input_pin_state[pin];
}

void digitalWrite(int pin, int value) {
	changeOccured = 1;
	output_pin_state[pin] = value;
}
