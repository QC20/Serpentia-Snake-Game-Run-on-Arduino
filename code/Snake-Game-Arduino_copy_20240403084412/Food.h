#ifndef Food_h
#define Food_h

#include <Arduino.h> // This library comes with the Arduino IDE
#include <Adafruit_GFX.h> // This library comes with the Arduino IDE
#include <Adafruit_PCD8544.h> // This library comes with the Arduino IDE
#include "SnakeGame.h"

class Food{
  public:
  	Food();
  	void randomize(byte width, byte height, byte weight);
  	void draw();
  	void execute();
  	int _x;
  	int _y;
  	int _index;
  	int points;
  	void initialize(Adafruit_PCD8544 *display);
  private:
  	Adafruit_PCD8544* _display;

};
#endif
