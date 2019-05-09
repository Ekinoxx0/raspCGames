#define TRUE 1
#define FALSE 0

#define	LOW 0
#define	HIGH 1

#define	INPUT			 0
#define	OUTPUT			 1

#define	INT_EDGE_SETUP		0
#define	INT_EDGE_FALLING	1
#define	INT_EDGE_RISING		2
#define	INT_EDGE_BOTH		3

#define X_BG_1 7
#define X_BG_2 0
#define X_BG_3 1
#define X_BG_4 2
#define X_BG_5 3
#define X_BG_6 4
#define X_BG_7 5
#define X_BG_8 6
#define X_BG_MAX 8

#define X_LED_GREEN 26
#define X_LED_RED 22

#define X_BUTTON_LEFT 21
#define X_BUTTON_RIGHT 29

extern int wiringPiSetup(void);
extern void pinMode(int pin, int mode);
extern int wiringPiISR(int pin, int mode, void (*function)(void));

extern          int  digitalRead(int pin);
extern          void digitalWrite(int pin, int value);

extern void         delay(unsigned int howLong);