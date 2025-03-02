#include <Adafruit_NeoPixel.h>

#include "WS_QMI8658.h"

#define RGB_PIN 14
#define RGB_NX 8
#define RGB_NY 8
#define RGB_COUNT (RGB_NX * RGB_NY)
#define RGB_Control_PIN 14

Adafruit_NeoPixel pixels(RGB_COUNT, RGB_Control_PIN, NEO_RGB + NEO_KHZ800);

struct Pt {
  int8_t x;
  int8_t y;

  bool operator==(const Pt& other) const {
    return this->x == other.x && this->y == other.y;
  }

  int ix() const { return this->y * RGB_NX + this->x; }
};

// Head is at 0, tail at snake_len-1.
static Pt snake[RGB_COUNT] = {0};
static int snake_len = 0;
static Pt snake_dir = {0};
static Pt food = {0};

void snake_init() {
  snake_len = 1;
  snake[0] = Pt{0, 0};
  snake_dir = Pt{1, 0};
  food = new_food_loc();
}

extern IMUdata Accel;
IMUdata game;

#define THRESHOLD 0.1

Pt gyro_dir(const Pt old) {
  if (old.x == 0) {
    if (Accel.y > THRESHOLD) {
      return Pt{-1, 0};
    }
    if (Accel.y < -THRESHOLD) {
      return Pt{1, 0};
    }
    return old;
  } else {
    if (Accel.x > THRESHOLD) {
      return Pt{0, 1};
    }
    if (Accel.x < -THRESHOLD) {
      return Pt{0, -1};
    }
    return old;
  }
}

int8_t mod8(int8_t a, int8_t div) {
  assert(div > 0);
  while (a < 0) a += div;
  return a % div;
}

Pt pt_move(Pt p, Pt dir) {
  return Pt{mod8(p.x + dir.x, RGB_NX), mod8(p.y + dir.y, RGB_NY)};
}

bool collides_with_snake(Pt p) {
  for (int i = 0; i < snake_len; i++) {
    if (snake[i] == p) {
      return true;
    }
  }
  return false;
}

Pt new_food_loc() {
  while (1) {
    const Pt loc = {random(0, RGB_NX), random(0, RGB_NY)};
    if (!collides_with_snake(loc)) {
      return loc;
    }
  }
}

void flash(const int n) {
  for (int i = 0; i < n; i++) {
    for (int x = 0; x < RGB_NX; x++) {
      for (int y = 0; y < RGB_NY; y++) {
        pixels.setPixelColor(x + y * RGB_NX, pixels.Color(100, 0, 0));
      }
    }
    pixels.show();
    delay(100);
    pixels.clear();
    pixels.show();
    delay(100);
  }
}

void snake_update() {
  // Change direction?
  snake_dir = gyro_dir(snake_dir);

  // Head moved into direction.
  const auto head = pt_move(snake[snake_len - 1], snake_dir);

  // Collided with food?
  if (food == head) {
    snake[snake_len++] = head;
    food = new_food_loc();
  } else {
    // Shift elements to left.
    for (int i = 1; i < snake_len; i++) {
      snake[i - 1] = snake[i];
    }
    snake[snake_len - 1] = head;
  }

  // Self-collision?
  for (int i = 0; i < snake_len - 1; i++) {
    if (snake[i] == head) {
      flash(5);
      delay(500);
      snake_init();
    }
  }
}

void render() {
  for (int i = 0; i < snake_len; i++) {
    const Pt p = snake[i];
    const auto color = (i == snake_len - 1) ? pixels.Color(128, 0, 0)
                                            : pixels.Color(128, 30, 0);
    const auto px_ix = p.y * RGB_NX + p.x;
    pixels.setPixelColor(px_ix, color);
  }

  pixels.setPixelColor(food.y * RGB_NX + food.x, pixels.Color(0, 128, 0));
}

void setup() {
  QMI8658_Init();

  pixels.begin();
  pixels.setBrightness(20);

  snake_init();
}

void loop() {
  QMI8658_Loop();

  snake_update();

  pixels.clear();
  render();
  pixels.show();

  delay(200 - snake_len * 7);
}
