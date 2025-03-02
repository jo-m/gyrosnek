#include <Adafruit_NeoPixel.h>

#define RGB_PIN 14
#define RGB_NX 8
#define RGB_NY 8
#define RGB_COUNT (RGB_NX * RGB_NY)
#define RGB_Control_PIN 14

Adafruit_NeoPixel pixels(RGB_COUNT, RGB_Control_PIN, NEO_RGB + NEO_KHZ800);

void anim_rainbow() {
  const auto ms = millis();
  for (int y = 0; y < RGB_NY; y++) {
    for (int x = 0; x < RGB_NX; x++) {
      const auto ix = y * RGB_NX + x;

      pixels.setPixelColor(ix,
                           pixels.ColorHSV((x + y) * 1000 + ms * 100, 255, 60));
    }
  }
  delay(1);
}

void anim_sparkle() {
  static uint16_t h[RGB_NY][RGB_NX] = {0};
  static uint8_t s[RGB_NY][RGB_NX] = {0};
  static float v[RGB_NY][RGB_NX] = {0};

  for (int y = 0; y < RGB_NY; y++) {
    for (int x = 0; x < RGB_NX; x++) {
      if (random(800) == 0) {
        h[y][x] = random(65536);
        s[y][x] = 128 + random(128);
        v[y][x] = 1;
      } else {
        v[y][x] *= 0.99;
      }

      const auto ix = y * RGB_NX + x;
      const auto color =
          pixels.ColorHSV(h[y][x], 255, pixels.gamma8(v[y][x] * 255));
      pixels.setPixelColor(ix, color);
    }
  }
  delay(1);
}

void setup() {
  pixels.begin();
  pixels.setBrightness(50);
}

void loop() {
  anim_sparkle();
  pixels.show();
}
