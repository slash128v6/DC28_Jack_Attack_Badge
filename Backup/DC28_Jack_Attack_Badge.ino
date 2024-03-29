// Visual Micro is in vMicro>General>Tutorial Mode
//
/*
Name:       DC28_Jack_Attack_Badge.ino
Created:	8/11/2019 9:25:15 PM
Author:     slash128
Notes:      Arduboy compatible
*/

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <EEPROM.h>
#include "sprites.h"

// Define User Types below here or use a .h file
//

// Hardware SPI OLED display settings
#define SCREEN_WIDTH	128
#define SCREEN_HEIGHT	64
#define OLED_DC     4
#define OLED_CS     12
#define OLED_RESET  6
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RESET, OLED_CS);

// Define Function Prototypes that use User Types below here or use a .h file
//
void initDisplay();
void initIO();
void rgbFlashLight();
boolean attractLoop();
void initGame();
void shipControl();
void laserControl();
void jackControl();
void explosionSound();
void collisionControl();
void gameOver();
void centerText(const char *Text, unsigned char Y);
void updateDisplay();
signed int backgroundControl(signed int shipMovX, signed int shipMovY);

// Define Functions below here or use other .ino or cpp files
//

// Global Variables and Defines
//
struct shipStruct ship;
struct laserStruct laser;
struct jackStruct jack;

const int shipWidth = 8;
const int shipHeight = 16;
const int shipSpeed = 4;
const int laserWidth = 8;
const int laserHeight = 2;
const int laserSpeed = 6;
const int jackWidth = 16;
const int jackHeight = 16;
const int explosionWidth = 16;
const int explosionHeight = 8;
const int bgrndWidth = 128;
const int bgrndHeight = 64;

signed int shipStartX = (SCREEN_WIDTH / 5);
signed int shipStartY = ((SCREEN_HEIGHT / 2) - (shipHeight / 2));
signed int playerLives = 0;
signed int jackPosX = SCREEN_WIDTH;
signed int jackPosY = ((SCREEN_HEIGHT / 2) - (jackHeight / 2));
signed int bgrndMovX;
signed int bgrndMovY;
float bgrnd1PosX = 0;
float bgrnd1PosY = 0;
float bgrnd2PosX = SCREEN_WIDTH;
float bgrnd2PosY = 0;
float bgrndMovSpeed = 0.1;

boolean laserFired = false;
boolean attractStatus = true;

unsigned int highScore = 0;
unsigned int playerScore = 0;
unsigned int explosionDuration = 50;

#define UP A0
#define RIGHT A1
#define LEFT A2
#define DOWN A3
#define BUTTA 7
#define BUTTB 8
#define BLUE 9 //PB5
#define RED 10 //PB6
#define GREEN 11 //PB7
#define BUZZER 5

#define ALIVE 0
#define HIT 1
#define DEAD 2

#define MAXLOOP 25

#define RGBCYCLE 50


// The setup() function runs once each time the micro-controller starts
void setup()
{
	initDisplay();
	initIO();
	initGame();
	
	EEPROM.get(0,highScore);
	if(highScore==65535)  // new unit never written to
	{
		highScore=0;
		EEPROM.put(0,highScore);
	}
	
	rgbFlashLight();
}

// Add the main program code into the continuous loop() function
void loop()
{
	
	if(attractStatus) {
		attractLoop();
	}
	
	shipControl();
	laserControl();
	jackControl();
	collisionControl();
	//backgroundControl(ship.locX, ship.locY);
	if(playerLives < 0) {
		delay(500);
		gameOver();
	}
	updateDisplay();
}

void initDisplay() {
	// Initialize the SPI OLED display
	display.begin(SSD1306_SWITCHCAPVCC);
	display.setTextSize(1);
	display.setTextColor(WHITE);
}

void initIO() {
	// Initialize I/O
	pinMode(UP, INPUT_PULLUP);
	pinMode(RIGHT, INPUT_PULLUP);
	pinMode(LEFT, INPUT_PULLUP);
	pinMode(DOWN, INPUT_PULLUP);
	pinMode(BUTTA, INPUT_PULLUP);
	pinMode(BUTTB, INPUT_PULLUP);
	pinMode(RED, OUTPUT);
	pinMode(GREEN, OUTPUT);
	pinMode(BUZZER, OUTPUT);
	analogWrite(RED, 255);
	analogWrite(GREEN, 255);
	analogWrite(BLUE, 255);
}

void rgbFlashLight() {
	int x = digitalRead(UP);
	if(!x) {
		while(1){
			display.clearDisplay();
			display.setTextSize(2);
			display.fillScreen(WHITE);
			display.setTextColor(BLACK);
			display.setCursor(40, 10);
			display.print("JACK");
			display.setCursor(20,40);
			display.print("ATTACKS!");
			display.display();
			analogWrite(RED, 0);
			display.clearDisplay();
			display.setTextSize(2);
			display.setTextColor(WHITE);
			display.setCursor(40, 10);
			display.print("JACK");
			display.setCursor(20,40);
			display.print("ATTACKS!");
			display.display();
			delay(RGBCYCLE);
			analogWrite(RED, 255);
			analogWrite(GREEN, 0);
			display.clearDisplay();
			display.setTextSize(2);
			display.fillScreen(WHITE);
			display.setTextColor(BLACK);
			display.setCursor(40, 10);
			display.print("JACK");
			display.setCursor(20,40);
			display.print("ATTACKS!");
			display.display();
			delay(RGBCYCLE);
			analogWrite(GREEN, 255);
			analogWrite(BLUE, 0);
			display.clearDisplay();
			display.setTextSize(2);
			display.setTextColor(WHITE);
			display.setCursor(40, 10);
			display.print("JACK");
			display.setCursor(20,40);
			display.print("ATTACKS!");
			display.display();
			delay(RGBCYCLE);
			analogWrite(BLUE, 255);
		}
	}
}

// Loop between Title and Press Start screens
boolean attractLoop() {

	int buttonState = 1;
	
	loopStart:

	// Title Screen
	for(int i = 0; i < MAXLOOP; i++) {

		display.clearDisplay();
		display.setTextSize(2);
		display.setTextColor(WHITE);
		display.setCursor(40, 10);
		display.print("JACK");
		display.setCursor(20,40);
		display.print("ATTACKS!");
		display.display();
		
		delay(RGBCYCLE);
		
		buttonState = digitalRead(BUTTB);
		if(!buttonState) {
			buttonState = 0;
			break;
		}
		
		display.clearDisplay();
		display.setTextSize(2);
		display.fillScreen(WHITE);
		display.setTextColor(BLACK);
		display.setCursor(40, 10);
		display.print("JACK");
		display.setCursor(20,40);
		display.print("ATTACKS!");
		display.display();
		
		delay(RGBCYCLE);

		buttonState = digitalRead(BUTTB);
		if(!buttonState) {
			buttonState = 0;
			break;
		}
	}

	if(!buttonState) {
		return attractStatus = false;
	}

	/**
	display.clearDisplay();
	display.setTextColor(WHITE);
	display.setTextSize(2);
	display.setCursor(40, 10);
	display.print("JACK");
	display.setCursor(20, 40);
	display.print("ATTACKS!");
	display.display();
	display.setTextSize(1);
	delay(100);
	buttonState = digitalRead(BUTTB);
	if(!buttonState) {
	buttonState = 0;
	break;
	}
	display.clearDisplay();
	display.display();
	delay(100);
	buttonState = digitalRead(BUTTB);
	if(!buttonState) {
	buttonState = 0;
	break;
	}
	}

	if(!buttonState) {
	return attractStatus = false;
	}
	**/

	// Press Start Screen
	for(int i = 0; i < MAXLOOP; i++) {
		display.clearDisplay();
		display.setTextColor(WHITE);
		display.setTextSize(1);
		centerText("Press Start!", 30);
		display.display();
		delay(100);
		buttonState = digitalRead(BUTTB);
		if(!buttonState) {
			buttonState = 0;
			break;
		}
		display.clearDisplay();
		display.display();
		delay(100);
		buttonState = digitalRead(BUTTB);
		if(!buttonState) {
			buttonState = 0;
			break;
		}
	}
	
	if(buttonState) {
		goto loopStart;
	}
	
	if(!buttonState) {
		return attractStatus = false;
	}
	
}

void initGame() {
	ship.locX = shipStartX;
	ship.locY = shipStartY;
	ship.status = ALIVE;
	
	laser.status = DEAD;
	
	playerLives = 3;
	
	jack.locX = jackPosX;
	jack.locY = random(8, 58);
	jack.speed = random(1, 5);
	jack.status = ALIVE;
}

void shipControl() {
	
	if(ship.status == DEAD) {
		ship.locX = shipStartX;
		ship.locY = shipStartY;
		ship.status = ALIVE;
	}
	
	if(digitalRead(UP) == 0) {
		ship.locY -= shipSpeed;
		if(ship.locY < 0) {
			ship.locY = 0;
		}
	}
	
	if(digitalRead(DOWN) == 0) {
		ship.locY += shipSpeed;
		if(ship.locY > (SCREEN_HEIGHT - (shipHeight/2))) {
			ship.locY = (SCREEN_HEIGHT - (shipHeight/2));
		}
	}
	
	if(digitalRead(LEFT) == 0) {
		ship.locX -= shipSpeed;
		if(ship.locX < 0) {
			ship.locX = 0;
		}
	}
	
	if(digitalRead(RIGHT) == 0) {
		ship.locX += shipSpeed;
		if(ship.locX > SCREEN_WIDTH) {
			ship.locX = SCREEN_WIDTH;
		}
	}
	
	if((digitalRead(BUTTA) == 0) && (laser.status == DEAD)) {
		laser.status = ALIVE;
		laser.locX = ship.locX;
		laser.locY = ship.locY + (shipHeight/2);
	}
}

void laserControl() {
	if(laser.status == ALIVE) {
		laser.locX += laserSpeed;
		if(laser.locX > SCREEN_WIDTH) {
			laser.status = DEAD;
		}
	}
}

void jackControl() {
	if(jack.status == DEAD) {
		jack.locX = SCREEN_WIDTH;
		jack.locY = random(8, 58);
		jack.speed = random(1, 5);
		jack.status = ALIVE;
	}
	
	if(jack.status == HIT) {
		jack.status = DEAD;
	}

	if(jack.status == ALIVE) {
		jack.locX -= jack.speed;
		if(jack.locX < 0) {
			jack.locX = SCREEN_WIDTH;
			jack.locY = random(8, 58);
			jack.speed = random(1, 5);
			jack.status == DEAD;
		}
	}
}

void explosionSound() {
	for(int i = 0; i < explosionDuration; i++) {
		analogWrite(RED, 127);
		delay(1);
		analogWrite(RED, 255);
		tone(BUZZER, 50, 10);
		analogWrite(GREEN, 127);
		delay(1);
		analogWrite(GREEN, 255);
		tone(BUZZER, 50, 10);
		analogWrite(BLUE, 127);
		delay(1);
		analogWrite(BLUE, 255);
		tone(BUZZER, 50, 10);
	}
}

void collisionControl() {
	
	// Laser and Jack Collision
	if((laser.status == ALIVE) && (jack.status == ALIVE)) {
		if((jack.locX <= (laser.locX + laserWidth)) && (laser.locX <= jack.locX)) {
			if(((laser.locY + laserHeight) >= jack.locY) && (laser.locY <= (jack.locY + jackHeight))) {
				laser.status = DEAD;
				jack.status = HIT;
				playerScore += 10;
				explosionSound();
			}
		}
	}
	
	// Ship and Jack Collision
	if((ship.status == ALIVE) && (jack.status == ALIVE)) {
		if((jack.locX <= (ship.locX + shipWidth)) && (ship.locX <= jack.locX)) {
			if(((ship.locY + shipHeight) >= jack.locY) && (ship.locY <= (jack.locY + jackHeight))) {
				ship.status = HIT;
				playerLives -= 1;
				jack.status = HIT;
				explosionSound();
			}
		}
	}
	
}

void gameOver() {
	if(playerScore > highScore) {
		highScore = playerScore;
		EEPROM.put(0,highScore);
	}
	
	int restart = 1;
	while(restart) {
		display.clearDisplay();
		display.setTextSize(1);
		display.setTextColor(WHITE);
		centerText("GAME OVER", 16);
		display.setCursor(29, 32);
		display.print("Score: ");
		display.print(playerScore);
		display.setCursor(4, 48);
		display.print("High Score: ");
		display.print(highScore);
		display.display();
		restart = digitalRead(BUTTB);
	}

	playerScore = 0;
	playerLives = 3;
	attractStatus = true;
}

signed int backgroundControl(signed int shipMovX, signed int shipMovY) {
	bgrndMovX = shipMovX;
	bgrndMovY = shipMovY;
	
	return bgrndMovX, bgrndMovY;
}

void centerText(const char *Text, unsigned char Y)  {
	// Centers text on screen
	display.setCursor(int((float)(SCREEN_WIDTH)/2-((strlen(Text)*6)/2)),Y);
	display.print(Text);
}

void updateDisplay() {
	display.clearDisplay();
	
	char buffer[21];
	
	// Lives and Score
	display.setCursor(0,0);
	

	if(playerScore < 10) {
		sprintf(buffer, "Lives: %d  Score:    %d", playerLives, playerScore);
		} else if(playerScore > 9 && playerScore < 100) {
		sprintf(buffer, "Lives: %d  Score:   %d", playerLives, playerScore);
		} else if(playerScore > 99 && playerScore < 1000) {
		sprintf(buffer, "Lives: %d  Score:  %d", playerLives, playerScore);
		} else if(playerScore > 10000) {
		sprintf(buffer, "Lives: %d  Score: %d", playerLives, playerScore);
	}


	for(int i = 0; i < playerLives; i++) {
		
		display.drawBitmap(((i*shipWidth)+2), (SCREEN_HEIGHT-shipHeight), shipBMP, shipWidth, shipHeight, WHITE);
	}
	

	
	display.print(buffer);
	
	if(ship.status == ALIVE) {
		display.drawBitmap(ship.locX, ship.locY, shipBMP, shipWidth, shipHeight, WHITE);
	}

	if(ship.status == HIT) {
		display.drawBitmap(ship.locX, ship.locY, explosionBMP, explosionWidth, explosionHeight, WHITE);
		ship.status = DEAD;
	}
	
	if(laser.status == ALIVE) {
		display.drawBitmap(laser.locX, laser.locY, laserBMP, laserWidth, laserHeight, WHITE);
	}
	
	if(jack.status == ALIVE) {
		display.drawBitmap(jack.locX, jack.locY, jackBMP, jackWidth, jackHeight, WHITE);
	}
	
	if(jack.status == HIT) {
		display.drawBitmap(jack.locX, jack.locY, explosionBMP, explosionWidth, explosionHeight, WHITE);
	}
	
	display.drawBitmap(bgrnd1PosX, bgrnd1PosY, bgrnd1BMP, bgrndWidth, bgrndHeight, WHITE);
	bgrnd1PosX -= bgrndMovSpeed;
	if(bgrnd1PosX < -127) {
		bgrnd1PosX = 0;
	}
	
	display.drawBitmap(bgrnd2PosX, bgrnd2PosY, bgrnd2BMP, bgrndWidth, bgrndHeight, WHITE);
	bgrnd2PosX -= bgrndMovSpeed;
	if(bgrnd2PosX < 0) {
		bgrnd2PosX = SCREEN_WIDTH;
	}
	
	display.display();
}