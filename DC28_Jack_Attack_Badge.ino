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
void initSAO();
boolean attractLoop();
void initGame();
void shipControl();
void laserFired();
void laserControl();
void jackControl();
void shipExplosion();
void jackExplosion();
void levelUp();
void bonusLife();
uint8_t collisionCheck(struct Object *obj1, struct Object *obj2);
void collisionControl();
void resetEEPROM();
void gameOver();
void centerText(const char *Text, unsigned char Y);
void updateDisplay();
void easterEgg();

// Define Functions below here or use other .ino or cpp files
//

// Global Variables and Defines
//
const uint8_t shipWidth = 8;
const uint8_t shipHeight = 16;
const uint8_t shipSpeed = 4;
const uint8_t laserWidth = 8;
const uint8_t laserHeight = 2;
const uint8_t laserSpeed = 6;
const uint8_t jackWidth = 16;
const uint8_t jackHeight = 16;
const uint8_t explosionWidth = 16;
const uint8_t explosionHeight = 8;
const uint8_t bgrndWidth = 128;
const uint8_t bgrndHeight = 64;

uint8_t shipStartX = (SCREEN_WIDTH / 5);
uint8_t shipStartY = ((SCREEN_HEIGHT / 2) - (shipHeight / 2));
int8_t playerLives;
uint8_t waveLevel;
uint16_t playerScore;
int16_t jackPosX = SCREEN_WIDTH;
int8_t jackPosY = ((SCREEN_HEIGHT / 2) - (jackHeight / 2));
uint8_t bgrndMovX;
uint8_t bgrndMovY;
uint8_t currentLaser;
uint8_t laserLevel;
uint8_t currentJack;
uint8_t jackLevel;
uint8_t shipandjackCollision = 0;
uint8_t laserandjackCollision = 0;
uint8_t maxLasers;
uint8_t maxJacks;
uint16_t highScore = 0;
uint8_t explosionDuration = 50;
uint16_t levelIncrement;
uint16_t bonusLifeScore;
float bgrnd1PosX = 0;
float bgrnd1PosY = 0;
float bgrnd2PosX = SCREEN_WIDTH;
float bgrnd2PosY = 0;
float bgrndMovSpeed = 0.1;

boolean attractStatus = true;
boolean bonusLifeEligible = false;
boolean levelUpEligible = false;

#define UP A0 //PF7
#define RIGHT A1 //PF6
#define LEFT A2 //PF5
#define DOWN A3 //PF4
#define JOYBUTT A4 //PF1
#define BUTTA 7
#define BUTTB 8
#define BLUE 9 //PB5
#define RED 10 //PB6
#define GREEN 11 //PB7
#define BUZZER 5

#define ALIVE 0
#define HIT 1
#define DEAD 2
#define MAXWAVES 9
#define MAXLASERS MAXWAVES
#define MAXJACKS MAXWAVES
#define ATTRACTLOOP 25
#define LEVELLOOP 10
#define RGBCYCLE 100

// SAO defines
#define SAO_VCC	2
#define SAO_GND	3

struct Object ship;
struct Object laser[MAXLASERS];
struct Object jack[MAXJACKS];

// The setup() function runs once each time the micro-controller starts
void setup()
{
	initDisplay();
	initIO();
	initSAO();
	initGame();
	
	EEPROM.get(0,highScore);
	if(highScore==65535)  // new unit never written to
	{
		highScore=0;
		EEPROM.put(0,highScore);
	}
	
}

// Add the main program code into the continuous loop() function
void loop()
{
	
	if(attractStatus) {
		attractLoop();
	}
	
	
	if(playerScore % levelIncrement == 0 && levelUpEligible == true) {
		levelUp();
	}
	
	
	if(playerScore % bonusLifeScore == 0 && bonusLifeEligible == true) {
		bonusLife();
	}
	
	shipControl();
	laserControl();
	jackControl();
	collisionControl();

	if(playerLives < 0) {
		delay(500);
		gameOver();
	}

	updateDisplay();

}

void initDisplay() {
	// Initialize hardware SPI OLED display
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
	pinMode(JOYBUTT, INPUT_PULLUP);
	pinMode(BUTTA, INPUT_PULLUP);
	pinMode(BUTTB, INPUT_PULLUP);
	pinMode(RED, OUTPUT);
	pinMode(GREEN, OUTPUT);
	pinMode(BUZZER, OUTPUT);
	for(uint8_t i = 9; i < 12; i++) {
		analogWrite(i, 255);
	}
}

void initSAO() {
	pinMode(SAO_VCC, OUTPUT);
	pinMode(SAO_GND, OUTPUT);
	digitalWrite(SAO_VCC, HIGH);
	digitalWrite(SAO_GND, LOW);
}

// Loop between Title and Press Start screens
boolean attractLoop() {

	uint8_t buttonState = 1;
	uint8_t ledActive = 9;
	
	while(1) {
		// Title Screen
		for(uint8_t i = 0; i < ATTRACTLOOP; i++) {

			display.clearDisplay();
			display.setTextSize(2);
			display.setTextColor(WHITE);
			display.setCursor(40, 10);
			display.print("JACK");
			display.setCursor(20,40);
			display.print("ATTACKS!");
			display.display();
			display.setTextSize(1);
			
			analogWrite(ledActive, 223);
			
			delay(RGBCYCLE);
			
			buttonState = digitalRead(BUTTB);
			if(!buttonState) {
				buttonState = 0;
				for(uint8_t j = 9; j < 12; j++) {
					analogWrite(j, 255);
				}
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
			display.setTextSize(1);
			
			analogWrite(ledActive, 255);
			
			delay(RGBCYCLE);

			ledActive += 1;
			if(ledActive > 11) {
				ledActive = 9;
			}

			buttonState = digitalRead(BUTTB);
			if(!buttonState) {
				buttonState = 0;
				for(uint8_t j = 9; j < 12; j++) {
					analogWrite(j, 255);
				}
				break;
			}
		}

		if(!buttonState) {
			return attractStatus = false;
		}

		// Press Start Screen
		for(uint8_t i = 0; i < ATTRACTLOOP; i++) {

			display.clearDisplay();
			display.setTextColor(WHITE);
			display.setTextSize(1);
			centerText("BUTTA = Fire!", 4);
			centerText("BUTTB = Start!", 16);
			centerText("Don't let Jack slip", 28);
			centerText("by to your base!", 38);
			centerText("Bonus Life Every 500", 52);
			display.display();

			for(uint8_t i = 9; i < 12; i++) {
				analogWrite(i, 223);
			}

			delay(100);
			buttonState = digitalRead(BUTTB);
			if(!buttonState) {
				buttonState = 0;
				for(uint8_t j = 9; j < 12; j++) {
					analogWrite(j, 255);
				}
				break;
			}

			display.display();

			for(uint8_t i = 9; i < 12; i++) {
				analogWrite(i, 255);
			}

			delay(100);
			buttonState = digitalRead(BUTTB);
			if(!buttonState) {
				buttonState = 0;
				for(uint8_t j = 9; j < 12; j++) {
					analogWrite(j, 255);
				}
				break;
			}
		}
		
		if(!buttonState) {
			for(uint8_t j = 9; j < 12; j++) {
				analogWrite(j, 255);
			}
			break;
		}
		
	}
	return attractStatus = false;
}

void initGame() {
	
	ship.locX = shipStartX;
	ship.sizeX = shipWidth;
	ship.locY = shipStartY;
	ship.sizeY = shipHeight;
	ship.speed = 1;
	ship.status = ALIVE;

	playerLives = 3;
	waveLevel = 0;
	playerScore = 0;
	levelIncrement = 100;
	bonusLifeScore = 500;
	bonusLifeEligible = false;
	levelUpEligible = true;
	attractStatus = true;
	
	currentLaser = 0;
	laserLevel = 0;
	for(uint8_t i = 0; i < MAXLASERS; i++) {
		laser[i].sizeX = laserWidth;
		laser[i].sizeY = laserHeight;
		laser[i].speed = laserSpeed;
		laser[i].status = DEAD;
	}

	
	currentJack = 0;
	jackLevel = 0;
	for(uint8_t i = 0; i < MAXJACKS; i++) {
		jack[i].locX = jackPosX;
		jack[i].sizeX = jackWidth;
		jack[i].locY = random(8, 58);
		jack[i].sizeY = jackHeight;
		jack[i].speed = random(1, 5);
		jack[i].status = ALIVE;
	}
	
}

void shipControl() {
	
	if(ship.status == DEAD) {
		ship.locX = shipStartX;
		ship.locY = shipStartY;
		ship.status = ALIVE;
	}
	
	if(digitalRead(UP) == 0) {
		ship.locY -= shipSpeed;
		if(ship.locY < 8) {
			ship.locY = 8;
		}
	}
	
	if(digitalRead(DOWN) == 0) {
		ship.locY += shipSpeed;
		if(ship.locY > (SCREEN_HEIGHT - shipHeight)) {
			ship.locY = (SCREEN_HEIGHT - shipHeight);
		}
	}
	
	if(digitalRead(LEFT) == 0) {
		ship.locX -= shipSpeed;
		if(ship.locX < 0) {
			ship.locX = SCREEN_WIDTH;
		}
	}
	
	if(digitalRead(RIGHT) == 0) {
		ship.locX += shipSpeed;
		if(ship.locX > SCREEN_WIDTH) {
			ship.locX = 0;
		}
	}
	

	if((digitalRead(BUTTA) == 0) && (laser[currentLaser].status == DEAD)) {
		laser[currentLaser].status = ALIVE;
		laser[currentLaser].locX = ship.locX;
		laser[currentLaser].locY = ship.locY + (shipHeight/2);
		currentLaser++;
		if(currentLaser > (laserLevel - 1)) {
			currentLaser = 0;
		}
		laserFired();
	}
	
}

void laserFired() {
	for(uint8_t i = 0; i < explosionDuration; i++) {
		analogWrite(GREEN, 127);
		delay(1);
		analogWrite(GREEN, 255);
	}
	tone(BUZZER, 750, 100);
}

void laserControl() {
	for(uint8_t i = 0; i < laserLevel; i++) {
		if(laser[i].status == ALIVE) {
			laser[i].locX += laserSpeed;
			if(laser[i].locX > SCREEN_WIDTH) {
				laser[i].status = DEAD;
			}
		}
	}
}

void jackControl() {

	for(uint8_t i = 0; i < jackLevel; i++) {

		if(jack[i].status == DEAD) {
			jack[i].locX = SCREEN_WIDTH;
			jack[i].locY = random(8, (SCREEN_HEIGHT - jackHeight));
			jack[i].speed = random(1, 5);
			jack[i].status = ALIVE;
		}
		
		if(jack[i].status == HIT) {
			jack[i].status = DEAD;
		}


		if(jack[i].status == ALIVE) {
			jack[i].locX -= jack[i].speed;
			if(jack[i].locX < 0) {
				jack[i].locX = SCREEN_WIDTH;
				jack[i].locY = random(8, (SCREEN_HEIGHT - jackHeight));
				jack[i].speed = random(1, 5);
				jack[i].status = DEAD;
				ship.status = HIT;
				playerLives -= 1;
				shipExplosion();
			}

		}
	}
}

void shipExplosion() {
	for(uint8_t i = 0; i < explosionDuration; i++) {
		analogWrite(RED, 127);
		delay(1);
		analogWrite(RED, 255);
	}
	tone(BUZZER, 50, 100);
}

void jackExplosion() {
	for(uint8_t i = 0; i < explosionDuration; i++) {
		analogWrite(BLUE, 127);
		delay(1);
		analogWrite(BLUE, 255);
	}
	tone(BUZZER, 50, 100);
}

void levelUp() {
	waveLevel++;
	if(waveLevel > MAXWAVES) {
		waveLevel = MAXWAVES;
	}
	laserLevel++;
	if(laserLevel > MAXLASERS) {
		laserLevel = MAXLASERS;
	}
	jackLevel++;
	if(jackLevel > MAXJACKS) {
		jackLevel = MAXJACKS;
	}
	levelUpEligible = false;
	
	if(waveLevel > 1) {
		levelIncrement = 200;
	}
	
	if(waveLevel > 3) {
		levelIncrement = 500;
	}
	
	if(waveLevel > 7) {
		levelIncrement = 1000;
	}
	
	
	char bufferLevel[21];
	
	uint8_t ledActive = 9;
	
	if(waveLevel == 1) {
		display.clearDisplay();
		display.setTextColor(WHITE);
		display.setTextSize(1);
		centerText("How many waves", 25);
		centerText("can you survive?!?", 40);
		display.display();
		delay(3000);
		display.clearDisplay();
	}
	
	for(uint8_t i = 0; i < LEVELLOOP; i++) {

		display.clearDisplay();
		display.setTextSize(2);
		display.setTextColor(WHITE);
		display.setCursor(45, 10);
		display.print("WAVE");
		display.setCursor(64,40);
		display.print(waveLevel);
		display.display();
		display.setTextSize(1);
		
		analogWrite(ledActive, 223);
		
		delay(RGBCYCLE);
		
		for(uint8_t j = 9; j < 12; j++) {
			analogWrite(j, 255);

		}
		

		display.clearDisplay();
		display.setTextSize(2);
		display.fillScreen(WHITE);
		display.setTextColor(BLACK);
		display.setCursor(45, 10);
		display.print("WAVE");
		display.setCursor(60,40);
		display.print(waveLevel);
		display.display();
		display.setTextSize(1);
		
		analogWrite(ledActive, 255);
		
		delay(RGBCYCLE);

		ledActive += 1;
		if(ledActive > 11) {
			ledActive = 9;
		}

		for(uint8_t j = 9; j < 12; j++) {
			analogWrite(j, 255);
		}

	}
	
	if(waveLevel == 9) {
		easterEgg();
	}
	
}

void bonusLife() {
	for(uint8_t i = 0; i < explosionDuration; i++) {
		for(uint8_t j = 9; j < 12; j++) {
			analogWrite(j, 127);
			delay(1);
			analogWrite(j, 255);
			tone(BUZZER, 200, 10);
		}
	}

	playerLives++;
	bonusLifeEligible = false;

}

uint8_t collisionCheck(struct Object *obj1, struct Object *obj2) {

	if(obj1->status == ALIVE && obj2->status ==ALIVE) {
		if(((obj2->locX < (obj1->locX + obj1->sizeX)) && ((obj2->locX + obj2->sizeX) > obj1->locX)) && ((obj2->locY < (obj1->locY + obj1->sizeY)) && ((obj2->locY + obj2->sizeY) > obj1->locY))) {
			return 1;
		}
	}
	return 0;
}

void collisionControl() {
	
	for(uint8_t i = 0; i < jackLevel; i++) {
		shipandjackCollision = collisionCheck(&ship, &jack[i]);
		if(shipandjackCollision) {
			ship.status = HIT;
			playerLives -= 1;
			jack[i].status = HIT;
			shipExplosion();
		}
	}
	
	for(uint8_t i = 0; i < laserLevel; i++) {
		for(uint8_t j = 0; j < jackLevel; j++){
			laserandjackCollision = collisionCheck(&laser[i], &jack[j]);
			if(laserandjackCollision) {
				laser[i].status = DEAD;
				jack[j].status = HIT;
				playerScore += 10;
				if(playerLives < 11) {
					bonusLifeEligible = true;
				}
				if(waveLevel < MAXWAVES) {
					levelUpEligible= true;
				}
				jackExplosion();
			}
		}
	}

}

void resetEEPROM() {
	
	uint8_t joyButt = 1;
	uint8_t joyLeft = 1;
	uint8_t joyRight = 1;
	uint8_t resetSelect = 0;
	boolean resetBest = false;
	
	while(joyButt) {

		joyButt = digitalRead(JOYBUTT);
		joyLeft = digitalRead(LEFT);
		joyRight = digitalRead(RIGHT);
		
		if(joyLeft == 0) {
			resetSelect = 1;
		}
		
		if(joyRight == 0) {
			resetSelect = 0;
		}
		
		if(resetSelect == 1) {
			
			display.setTextColor(WHITE);
			display.setTextSize(1);
			display.setCursor(12, 0);
			display.print("Reset Best Score?");
			display.setCursor(32, 20);
			display.print("Yes     No");
			display.drawFastHLine(32, 30, 17, WHITE);
			
			display.setCursor(10, 40);
			display.print("JoyLeft / JoyRight");
			display.setCursor(12, 54);
			display.print("JoyButt to Select");
			
			resetBest = true;
		}
		
		if(resetSelect == 0) {
			
			display.setTextColor(WHITE);
			display.setTextSize(1);
			display.setCursor(12, 0);
			display.print("Reset Best Score?");
			display.setCursor(32, 20);
			display.print("Yes     No");
			display.drawFastHLine(79, 30, 12, WHITE);
			
			display.setCursor(10, 40);
			display.print("JoyLeft / JoyRight");
			display.setCursor(12, 54);
			display.print("JoyButt to Select");
			
			resetBest = false;
		}
		
		
		display.display();
		
		if(resetBest) {
			//Reset EEPROM
			highScore=0;
			EEPROM.put(0,highScore);
		}
		
		joyButt = digitalRead(JOYBUTT);
		
		display.clearDisplay();
		
	}
}

void gameOver() {
	if(playerScore > highScore) {
		highScore = playerScore;
		EEPROM.put(0,highScore);
	}
	
	uint8_t restart = 1;
	while(restart) {
		display.clearDisplay();
		display.setTextSize(1);
		display.setTextColor(WHITE);
		centerText("GAME OVER", 12);
		display.setCursor(30, 28);
		display.print("Score: ");
		display.print(playerScore);
		display.setCursor(35, 44);
		display.print("Best: ");
		display.print(highScore);
		display.display();
		restart = digitalRead(BUTTB);
	}

	resetEEPROM();
	
	initGame();

}

void easterEgg() {
	int8_t pacmanOffset = 20;
	int8_t ghostOffset = 50;
	uint8_t frameDelay = 10;
	uint8_t frameCount = 10;
	uint8_t frameFlag = 1;
	uint8_t frameSelect = 1;
	
	for(int16_t i = 0; i < 190; i++) {
		display.clearDisplay();
		
		switch(frameSelect) {
			case 1:
			display.drawBitmap((i - pacmanOffset), 20, pacmanFrame1BMP, 16, 16, WHITE);
			frameFlag++;
			if(frameFlag == frameCount) {
				frameSelect = 2;
				frameFlag = 0;
			}
			break;
			
			case 2:
			display.drawBitmap((i - pacmanOffset), 20, pacmanFrame2BMP, 16, 16, WHITE);
			frameFlag++;
			if(frameFlag == frameCount) {
				frameSelect = 1;
				frameFlag = 0;
			}
			break;
		}
		
		display.drawBitmap((i - ghostOffset), 20, ghostBMP, 16, 16, WHITE);
		
		display.display();
		
		delay(frameDelay);

	}
	
	delay(1000);
	
	for(int16_t i = 190; i > 0; i--) {
		display.clearDisplay();
		
		switch(frameSelect) {
			case 1:
			display.drawBitmap((i - pacmanOffset), 20, pacmanFrame1BMP, 16, 16, WHITE);
			frameFlag++;
			if(frameFlag == frameCount) {
				frameSelect = 2;
				frameFlag = 0;
			}
			break;
			
			case 2:
			display.drawBitmap((i - pacmanOffset), 20, pacmanFrame3BMP, 16, 16, WHITE);
			frameFlag++;
			if(frameFlag == frameCount) {
				frameSelect = 1;
				frameFlag = 0;
			}
			break;
		}
		
		display.drawBitmap((i - ghostOffset), 20, ghostBMP, 16, 16, WHITE);
		
		display.display();
		
		delay(frameDelay);

	}
	
	delay(1000);
	
}

void centerText(const char *Text, unsigned char Y)  {
	display.setCursor(int((float)(SCREEN_WIDTH)/2-((strlen(Text)*6)/2)),Y);
	display.print(Text);
}

void updateDisplay() {
	display.clearDisplay();
	
	char bufferScore[21];
	
	// Display score
	display.setCursor(0,0);
	display.setTextSize(1);
	display.setTextColor(WHITE);

	if(playerScore < 10) {
		sprintf(bufferScore, "                    %d", playerScore);
	}
	
	if(playerScore >= 10 && playerScore <= 99) {
		sprintf(bufferScore, "                   %d", playerScore);
	}
	
	if(playerScore >= 100 && playerScore <= 999) {
		sprintf(bufferScore, "                  %d", playerScore);
	}
	
	if(playerScore >= 1000) {
		sprintf(bufferScore, "                 %d", playerScore);
	}
	
	if(playerScore >= 10000) {
		sprintf(bufferScore, "                %d", playerScore);
	}
	
	display.print(bufferScore);
	
	// Display variables for debugging
	/**
	display.setCursor(0, 8);
	display.setTextColor(BLACK);
	display.print(currentLaser);
	display.setTextColor(WHITE);
	**/
	
	// Draw remaining lives
	for(uint8_t i = 0; i < playerLives; i++) {
		display.drawBitmap(i*((shipWidth/2)+2), 0, shipSmallBMP, shipWidth/2, shipHeight/2, WHITE);
	}
	
	// Draw ship
	if(ship.status == ALIVE) {
		display.drawBitmap(ship.locX, ship.locY, shipBMP, shipWidth, shipHeight, WHITE);
	}

	if(ship.status == HIT) {
		display.drawBitmap(ship.locX, ship.locY, explosionBMP, explosionWidth, explosionHeight, WHITE);
		ship.status = DEAD;
	}
	
	// Draw lasers
	for(uint8_t i = 0; i < laserLevel; i++) {
		if(laser[i].status == ALIVE) {
			display.drawBitmap(laser[i].locX, laser[i].locY, laserBMP, laserWidth, laserHeight, WHITE);
		}
	}
	
	// Draw jacks
	for(uint8_t i = 0; i < jackLevel; i++) {
		if(jack[i].status == ALIVE) {
			display.drawBitmap(jack[i].locX, jack[i].locY, jackBMP, jackWidth, jackHeight, WHITE);
		}

		if(jack[i].status == HIT) {
			display.drawBitmap(jack[i].locX, jack[i].locY, explosionBMP, explosionWidth, explosionHeight, WHITE);
		}
	}
	
	// Draw background
	display.drawBitmap(bgrnd1PosX, bgrnd1PosY+8, bgrnd1BMP, bgrndWidth, bgrndHeight, WHITE);
	bgrnd1PosX -= bgrndMovSpeed;
	if(bgrnd1PosX < -127) {
		bgrnd1PosX = 0;
	}

	display.drawBitmap(bgrnd2PosX, bgrnd2PosY+8, bgrnd2BMP, bgrndWidth, bgrndHeight, WHITE);
	bgrnd2PosX -= bgrndMovSpeed;
	if(bgrnd2PosX < 0) {
		bgrnd2PosX = SCREEN_WIDTH;
	}
	
	display.display();
}