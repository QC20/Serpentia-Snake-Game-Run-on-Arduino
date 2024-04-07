#include <Blocks.h>
#include <Food.h>
#include <GameManager.h>
#include <Snake.h>
#include <SnakeGame.h>
#include <pitches.h>
#include <EEPROM.h>
#include <U8g2lib.h>

// Bitmap image
static const unsigned char startScreenBitmap[] = {
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000
};

// (CLK,DIN,D/C,CE,RST);
Adafruit_PCD8544 display = Adafruit_PCD8544(13, 11, 9, 10, 8);

// Game objects
Snake snake = Snake();
Food food = Food();
GameManager gameManager = GameManager();

// Buttons
volatile boolean up = false;
volatile boolean down = false;
volatile boolean right = false;
volatile boolean left = false;

int downButtonState = 0;
int upButtonState = 0;
int rightButtonState = 0;
int leftButtonState = 0;

int lastLeftButtonState = 0;
int lastDownButtonState = 0;
int lastRightButtonState = 0;
int lastUpButtonState = 0;

// Initialize LCD
boolean backlight = true;
int contrast = LCD_INIT_CONTRAST;
int menuitem = 1;
int page = MENU_SCREEN;

int sound = true;

int highscore = 0;

void setup() {
  Serial.begin(9600);

  // Initialize button pins as input with pull-up resistors
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_LEFT, INPUT_PULLUP);

  // Initialize LCD backlight pin as output
  pinMode(LCD_BACKLIGHT_PIN, OUTPUT);

  // Read highscore from EEPROM
  highscore = readIntFromEEPROM(HIGHSCORE_ADDRESS);

  // Seed random number generator
  randomSeed(analogRead(0));

  // Initialize LCD display
  display.begin();
  display.setContrast(LCD_INIT_CONTRAST);
  display.clearDisplay();
  display.display();

  // Display start screen image
  display.drawBitmap(0, 0, startScreenBitmap, 84, 48, 1);
  display.print("Snake Game");
  display.display();

  // Delay for 3 seconds
  delay(3000);

  // Clear the display before starting the game
  display.clearDisplay();
  display.display();

  // Initialize game manager with display, snake, and food objects
  gameManager.initialize(&display, &snake, &food);
}

void loop() {
  // Clear LCD display and draw menu
  display.clearDisplay();
  drawMenu();

  // Read button states
  downButtonState = digitalRead(BUTTON_DOWN);
  rightButtonState = digitalRead(BUTTON_RIGHT);
  upButtonState = digitalRead(BUTTON_UP);
  leftButtonState = digitalRead(BUTTON_LEFT);

  // Check button presses
  checkIfDownButtonIsPressed();
  checkIfUpButtonIsPressed();
  checkIfSelectButtonIsPressed();
  checkIfBackButtonIsPressed();

  // Handle button actions
  handleBackButton();
  handlePages();
}

// Handle game logic
void gameHandler() {
  if (!gameManager.gameOver) {
    if (!upButtonState) {
      gameManager.moveSnake(1);
    } else if (!rightButtonState) {
      gameManager.moveSnake(3);
    } else if (!leftButtonState) {
      gameManager.moveSnake(4);
    } else if (!downButtonState) {
      gameManager.moveSnake(2);
    }
    gameManager.execute();
  } else {
    gameOverScreen();

    if (!rightButtonState) {
      resetGame();
      page = 1;
    }
  }
  delay(70);
}

// Reset game state
void resetGame() {
  snake = Snake();
  food = Food();
  gameManager = GameManager();
  gameManager.initialize(&display, &snake, &food);
}

// Display game over screen
void gameOverScreen() {
  display.setTextSize(1);
  display.setCursor(17, 0);
  display.println("GAME OVER!");
  display.setTextSize(1);
  display.setCursor(10, 24);
  highscore = readIntFromEEPROM(HIGHSCORE_ADDRESS);


  if (gameManager.score >= highscore) {
    display.print("HIGHSCORE! ");
    writeIntIntoEEPROM(HIGHSCORE_ADDRESS, gameManager.score);
  } else {
    display.print("Score: ");
  }
  display.println(gameManager.score);
  display.display();
}

// Handle back button action
void handleBackButton() {
  if (page != GAME_SCREEN) {
    if (left && page == HIGHSCORE_SCREEN) {
      left = false;
      page = 1;
    } else if (left && page > MENU_SCREEN) {
      left = false;
      page--;
    }
  }
}

// Handle page navigation
void handlePages() {
  if (page == MENU_SCREEN) {
    handleFirstPage();
  } else {
    handleSecondPage();
  }
}

// Handle menu navigation on the first page
void handleFirstPage() {
  // Move cursor up
  if (up && page == MENU_SCREEN) {
    up = false;
    menuitem--;
    if (menuitem == 0) {
      menuitem = 3;
    }
  } else if (up && page == SETTINGS_SCREEN) {
    up = false;
  }

  // Move cursor down
  if (down && page == MENU_SCREEN) {
    down = false;
    menuitem++;
    if (menuitem == 4) {
      menuitem = 1;
    }
  } else if (down && page == SETTINGS_SCREEN) {
    down = false;
  }

  if (right && page == MENU_SCREEN) {
    right = false;

    if (page == MENU_SCREEN && menuitem == 1) {
      page = GAME_SCREEN;
    } else if (page == MENU_SCREEN && menuitem == 2) {
      page = HIGHSCORE_SCREEN;
    } else if (page == MENU_SCREEN && menuitem == 3) {
      menuitem = 1;
      page = SETTINGS_SCREEN;
    }
  }
}

// Handle menu navigation on the second page (Settings)
void handleSecondPage() {
  if (up && page == SETTINGS_SCREEN) {
    up = false;
    menuitem--;
    if (menuitem == 0) {
      menuitem = 3;
    }
  } else if (up && page == SETTINGS_SCREEN) {
    up = false;
    contrast--;
    setContrast();
  }

  // Move cursor down 
  if (down && page == SETTINGS_SCREEN) {
    down = false;
    menuitem++;
    if (menuitem == 4) {
      menuitem = 1;
    }
  } else if (down && page == CONTRAST_SCREEN) {
    down = false;
    contrast++;
    setContrast();
  }

  // Handle setting changes 
  if (right) {
    right = false;
    if (page == SETTINGS_SCREEN && menuitem == 2) {
      if (backlight) {
        backlight = false;
        setBacklight(LCD_BACKLIGHT_OFF);
      } else {
        backlight = true;
        setBacklight(LCD_BACKLIGHT_ON);
      }
    }

    if (page == SETTINGS_SCREEN && menuitem == 3) {
      if (sound) {
        sound = false;
        gameManager.setSound(LOW);
      } else {
        sound = true;
        gameManager.setSound(HIGH);
      }
    } else if (page == SETTINGS_SCREEN && menuitem == 1) {
      page = CONTRAST_SCREEN;
    } else if (page == CONTRAST_SCREEN) {
      page = SETTINGS_SCREEN;
    }
  }
}

// Draw menu on the LCD display 
void drawMenu() {
  // Draw menu items based on current page 
  if (page == MENU_SCREEN) {
    display.setTextSize(1);
    display.clearDisplay();
    display.setTextColor(BLACK, WHITE);
    display.setCursor(15, 0);
    display.print("Snake Game");
    display.drawFastHLine(0, 10, 83, BLACK);
    display.setCursor(0, 15);

    // Draw menu items for the main manu
    if (menuitem == 1) {
      display.setTextColor(WHITE, BLACK);
    } else {
      display.setTextColor(BLACK, WHITE);
    }
    display.print(">Play");
    display.setCursor(0, 25);

    // Deaw menu items for settings
    if (menuitem == 2) {
      display.setTextColor(WHITE, BLACK);
    } else {
      display.setTextColor(BLACK, WHITE);
    }
    display.print(">Highscore");

    // Draw menu items for contrast 
    if (menuitem == 3) {
      display.setTextColor(WHITE, BLACK);
    } else {
      display.setTextColor(BLACK, WHITE);
    }
    display.setCursor(0, 35);
    display.print(">Settings");
    display.display();
  }

  else if (page == SETTINGS_SCREEN) {
    // Draw menu items for settings 
    display.setTextSize(1);
    display.clearDisplay();
    display.setTextColor(BLACK, WHITE);
    display.setCursor(15, 0);
    display.print("Settings");
    display.drawFastHLine(0, 10, 83, BLACK);
    display.setCursor(0, 15);

    // Draw constrast option
    if (menuitem == 1) {
      display.setTextColor(WHITE, BLACK);
    } else {
      display.setTextColor(BLACK, WHITE);
    }
    display.print(">Contrast");
    display.setCursor(0, 25);

    // Draw light option
    if (menuitem == 2) {
      display.setTextColor(WHITE, BLACK);
    } else {
      display.setTextColor(BLACK, WHITE);
    }
    display.print(">Light: ");

    // Indicate current light state
    if (backlight) {
      display.print("ON");
    } else {
      display.print("OFF");
    }

    // Draw sound option
    if (menuitem == 3) {
      display.setTextColor(WHITE, BLACK);
    } else {
      display.setTextColor(BLACK, WHITE);
    }

    display.setCursor(0, 35);
    display.print(">Sound: ");
    // Indicate current sound state
    if (sound) {
      display.print("ON");
    } else {
      display.print("OFF");
    }

    display.display();
  } else if (page == CONTRAST_SCREEN) {
    // Draw menu items for contrast
    display.setTextSize(1);
    display.clearDisplay();
    display.setTextColor(BLACK, WHITE);
    display.setCursor(15, 0);
    display.print("CONTRAST");
    display.drawFastHLine(0, 10, 83, BLACK);
    display.setCursor(5, 15);
    display.print("Value");
    display.setTextSize(2);
    display.setCursor(5, 25);
    display.print(contrast);

    display.setTextSize(2);
    display.display();
  } else if (page == HIGHSCORE_SCREEN) {
    // Draw highscore screen
    display.setTextSize(1);
    display.clearDisplay();
    display.setTextColor(BLACK, WHITE);
    display.setCursor(15, 0);
    display.print("HIGHSCORE:");
    display.drawFastHLine(0, 10, 83, BLACK);
    display.setCursor(5, 15);
    display.print("/n Value");
    display.setTextSize(2);
    display.setCursor(5, 25);

    int number = readIntFromEEPROM(HIGHSCORE_ADDRESS);
    // Read highscore from EEPROM and display 
    display.print(number);
    display.setTextSize(2);
    display.display();
  } else if (page == GAME_SCREEN) {
    // Handle gameplay
    gameHandler();
  }
}

void resetDefaults() {
  // Reset contrast and backlight setting to defaults
  contrast = LCD_INIT_CONTRAST;
  setContrast();
  backlight = true;
  setBacklight(HIGH);
}

void setContrast() {
  // Set the contrast level of the display 
  display.setContrast(contrast);
  display.display();
}

void setBacklight(int state) {
  // Set the state of the LED backlight 
  digitalWrite(LCD_BACKLIGHT_PIN, state);
}

// R/W EEPROM
void writeIntIntoEEPROM(int address, int number) {
  EEPROM.write(address, number >> 8);
  EEPROM.write(address + 1, number & 0xFF);
}

int readIntFromEEPROM(int address) {
  // Read interger from EEPROM memory
  byte byte1 = EEPROM.read(address);
  byte byte2 = EEPROM.read(address + 1);
  return (byte1 << 8) + byte2;
}

//Button functions
void checkIfDownButtonIsPressed() {
  // Check if DOWN button is pressed 
  if (downButtonState != lastDownButtonState) {
    if (downButtonState == 0) {
      down = true;
    }
    delay(50);
  }
  lastDownButtonState = downButtonState;
}

void checkIfBackButtonIsPressed() {
  // Check if BACK button is pressed
  if (leftButtonState != lastLeftButtonState) {
    if (leftButtonState == 0) {
      left = true;
    }
    delay(50);
  }
  lastLeftButtonState = leftButtonState;
}

void checkIfUpButtonIsPressed() {
  // Check if UP button is pressed
  if (upButtonState != lastUpButtonState) {
    if (upButtonState == 0) {
      up = true;
    }
    delay(50);
  }
  lastUpButtonState = upButtonState;
}

void checkIfSelectButtonIsPressed() {
  // Check if SELECT button is pressed
  if (rightButtonState != lastRightButtonState) {
    if (rightButtonState == 0) {
      right = true;
    }
    delay(50);
  }
  lastRightButtonState = rightButtonState;
}