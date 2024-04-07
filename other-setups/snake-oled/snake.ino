#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 oled(128, 64, &Wire, -1);

#define RIGHT 0
#define LEFT  1
#define UP    2
#define DOWN  3

//STICKBOX Pin
#define pinX  A0
#define pinY  A1

int valueX = 0;
int valueY = 0;
unsigned char keyValue = 0;

// Snake Value
const uint8_t block[] PROGMEM = {
  0xf0, // Binary: 11110000
  0xb0, // Binary: 10110000
  0xd0, // Binary: 11010000
  0xf0, // Binary: 11110000
};

uint8_t snake_head_x = 4;
uint8_t snake_head_y = 4;
uint8_t x[100];
uint8_t y[100];
uint8_t snake_len = 2;
uint8_t snake_dir = RIGHT;

uint8_t food_x;
uint8_t food_y;
bool food_eaten = true;

bool game_over = false;
int score = 0;
int level = 1;
int snake_speed = 150;
int i;

// Function to scan the joystick
void keyScan(void)
{
  static unsigned char keyUp = 1;

  valueX = analogRead(pinX);
  valueY = analogRead(pinY);

  if (keyUp && ((valueX <= 10) || (valueX >= 1010) || (valueY <= 10) || (valueY >= 1010)))
  {
    delay(10);
    keyUp = 0;
    if (valueX <= 10)
    {
      if (snake_dir != UP)
      {
        snake_dir = DOWN;
      }
    }
    else if (valueX >= 1010)
    {
      if (snake_dir != DOWN)
      {
        snake_dir = UP;
      }
    }
    else if (valueY <= 10)
    {
      if (snake_dir != RIGHT)
      {
        snake_dir = LEFT;
      }
    }
    else if (valueY >= 1010)
    {
      if (snake_dir != LEFT)
      {
        snake_dir = RIGHT;
      }
    }
  } else if ((valueX > 10) && (valueX < 1010) && (valueY > 10) && (valueY < 1010)) keyUp = 1;
  return 0;
}

void draw_snake(int x, int y)
{
  oled.drawBitmap(x, y, block, 4, 4, 1);
}

void show_score(int x, int y, int data)
{
  oled.setCursor(x, y);
  oled.println(data);
}

void screen(void)
{
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.drawRect(0, 1, 102, 62, 1);
  oled.drawRect(0, 0, 102, 64, 1);
  oled.setCursor(104, 12);
  oled.println("LEVEL");
  oled.setCursor(104, 40);
  oled.println("SCORE");

  show_score(110, 25, level);
  show_score(110, 53, score);

  for (i = 0; i < snake_len; i++)
  {
    draw_snake(x[i], y[i]);
  }

  draw_snake(food_x, food_y);

  oled.display();
}

void draw_food(void)
{
  int food_out = 0;

  if (food_eaten)
  {
    while (food_out == 0)
    {
      food_out = 1;

      food_x = (uint8_t)(random(4, 100) / 4) * 4;
      food_y = (uint8_t)(random(4, 60) / 4) * 4;

      for (int i = snake_len - 1; i > 0; i--)
      {
        if (food_x == x[i] && food_y == y[i])
        {
          food_out = 0;
        }
      }
    }
  }

  food_eaten = false;
}

void snake_move(void)
{
  switch (snake_dir) {
    case RIGHT:
      snake_head_x += 4;
      break;
    case UP:
      snake_head_y -= 4;
      break;
    case LEFT:
      snake_head_x -= 4;
      break;
    case DOWN:
      snake_head_y += 4;
      break;
  }

  if ((snake_head_x == food_x) && (snake_head_y == food_y))
  {
    food_eaten = true;
    snake_len++;
    score++;
    level = score / 5 + 1;
    snake_speed -= level;
  }

  for (i = snake_len - 1; i > 0; i--)
  {
    x[i] = x[i - 1];
    y[i] = y[i - 1];
  }
  x[0] = snake_head_x;
  y[0] = snake_head_y;

  check_snake_die();
}

void draw_game_over()
{
  oled.clearDisplay();
  oled.setTextSize(2);
  oled.setCursor(10, 10);
  oled.println("GAME OVER");
  oled.setTextSize(1);
  oled.setCursor(30, 35);
  oled.println("LEVEL:");
  oled.setCursor(30, 55);
  oled.println("SCORE:");

  show_score(80, 35, level);
  show_score(80, 55, score);

  oled.display();
}

void check_snake_die(void)
{
  // Hit wall
  if (snake_head_x < 4 || snake_head_x > 96 || snake_head_y < 1 || snake_head_y > 56)
  {
    game_over = true;
  }

  // Eat itself
  if (snake_len > 4)
  {
    for (int i = 1; i < snake_len; i++)
    {
      if (snake_head_x == x[i] && snake_head_y == y[i])
      {
        game_over = true;
      }
    }
  }
}

void setup()
{
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  oled.setTextColor(WHITE);
  randomSeed(analogRead(3));
}

void loop()
{
  if (game_over)
  {
    draw_game_over();
  } else {
    keyScan();
    snake_move();
    draw_food();
    screen();
  }
  delay(snake_speed);
}
