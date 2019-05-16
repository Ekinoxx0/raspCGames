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

void setupBargraph();
void setupLeds();
void setupButtons();

void writeGreenLed(int state);
void writeRedLed(int state);
void toggleGreenLed();
void toggleRedLed();
void writeBargraph(unsigned char stateLedsBG);
void writeBargraphUniLed(int led);
void blankBargraph();
void fullBargraph();
int readLeftButton();
int readRightButton();

double pow(double x, double y);
float powf(float x, float y);
long double powl(long double x, long double y);