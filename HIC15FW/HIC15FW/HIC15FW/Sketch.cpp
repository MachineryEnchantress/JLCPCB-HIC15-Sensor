
/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

/*End of auto generated code by Atmel studio */

/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>
#include <string.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>



#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
// D0=13, D1=11, CS=5, DC=4, Reset=3
#define OLED_MOSI  11
#define OLED_CLK   13
#define OLED_DC    4
#define OLED_CS    5
#define OLED_RESET 3

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
void oled_display(float hic15);

#define  DEBUG_TERMINAL 1
#define DISPLAY_FLASH_RATE 1000  // display update every 1000mS


#define XOUT A0
#define YOUT A1
#define ZOUT A2


#define RANK    15

float scale = 200;
float maxHIC15;
int	  tcount = 0;
float mapf(int x);
float getAccel(float x, float y, float z);
float getHIC15(float);
void dispHIC15(float hic15);
void pushADCData();
float getSumHIC15();
float getAccelData();
uint8_t  HIC15Handler();
void getMaxHICData();

float scaledX, scaledY, scaledZ, hic15;
int rawX[RANK], rawY[RANK], rawZ[RANK];
float ac[RANK];

uint8_t cntBuf;

uint8_t isPowerOn = 1;
uint8_t btnCnt = 0;



void initBuffer(){
	memset(ac, 0, sizeof(float)*RANK);
	memset(rawX, 0, sizeof(int)*RANK);
	memset(rawY, 0, sizeof(int)*RANK);
	memset(rawZ, 0, sizeof(int)*RANK);
	maxHIC15 = 0;
	tcount = 0;
	cntBuf = 0;
}

void setup() {
		
	pinMode(2, OUTPUT);
	pinMode(8, INPUT);

	Serial.begin(9600);
	// SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
	if(!display.begin(SSD1306_SWITCHCAPVCC)) {	
		for(;;); // Don't proceed, loop forever
	}
	initBuffer();
}


void loop() {
	
	long tempTime = micros();
	pushADCData();
	if (HIC15Handler() == 1) delayMicroseconds(1000 - (micros() - tempTime));

}


void powerHandler(){
	Serial.print(digitalRead(8));
	if (digitalRead(8) == LOW) btnCnt++;
	else {
		btnCnt = 0;
		isPowerOn = 0;
	}
	Serial.println(btnCnt);
	if (btnCnt > 3)
	{
		digitalWrite(2, LOW);
	}
	else
	{
		digitalWrite(2, HIGH);
	}
}
uint8_t HIC15Handler(){
	tcount ++;
	if (tcount > DISPLAY_FLASH_RATE) {
		powerHandler();
		maxHIC15 = getHIC15(maxHIC15);	
/*		Serial.print("=============");	
		Serial.print((int)maxHIC15); Serial.print(".");Serial.println((int)(maxHIC15*100)%100); 
		*/
		oled_display(maxHIC15);		
		initBuffer();	
		return 0;
	}
	getMaxHICData();
	return 1;
}

float getAccel(float x, float y, float z){

	double sum = x*x + y*y + z*z;
	
	return sqrt(sum);
}


float getAccelData(){
	
	scaledX = rawX[cntBuf] /2.56 - 200;
	scaledY = rawY[cntBuf] /2.56 - 200;
	scaledZ = rawZ[cntBuf] /2.56 - 200;	

	return getAccel(scaledX, scaledY, scaledZ);
}


void pushADCData(){
	rawX[cntBuf] = analogRead(XOUT);
	rawY[cntBuf] = analogRead(YOUT);
	rawZ[cntBuf] = analogRead(ZOUT);
/*	Serial.print(rawX[cntBuf]); Serial.print(",");
	Serial.print(rawY[cntBuf]); Serial.print(",");
	Serial.print(rawZ[cntBuf]); Serial.print(",");*/
	ac[cntBuf]   = getAccelData();
	
	cntBuf ++;
	cntBuf %=  RANK;
}

float getHIC15(float hic15){
	
	return  pow(hic15/RANK , 2.5)/1000 * RANK;
	
}
float getSumHIC15(){
	float hic15 = ac[0]/2;
	hic15 += ac[RANK - 1] /2;	
	for(uint8_t i = 1; i < RANK-1; i++)
	{
		hic15 += ac[i];
	}
	return hic15;
}
void getMaxHICData(){
	float bufData = getSumHIC15();
	if (bufData >  maxHIC15) maxHIC15 = bufData;
/*	
	Serial.print((int)bufData); Serial.print(".");Serial.print((int)(bufData*100)%100); Serial.print(",");
	Serial.print((int)maxHIC15); Serial.print(".");Serial.println((int)(maxHIC15*100)%100); 
	*/
	
}
void oled_display(float hic15)
{
	char dispstr[20];
	memset(dispstr,0,20);
	
	display.clearDisplay();
	display.setTextSize(3);      // Normal 1:1 pixel scale
	display.setTextColor(WHITE); // Draw white text
	display.setCursor(5, 25);     // Start at top-left corner
	display.cp437(true);         // Use full 256 char 'Code Page 437' font

	sprintf(dispstr, "%d.%02d", (int)hic15, (int)(hic15*100)%100);

	
	for(int16_t i=0; i < strlen(dispstr); i++) {
		display.write(dispstr[i]);
	}
	display.display();
	
	//delay(10);
}
