#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#define TFT_CS   4
#define TFT_DC   8
#define TFT_RST  7   // если RST не подключён: Adafruit_ILI9341 tft(TFT_CS, TFT_DC);

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  tft.begin();
  tft.setRotation(3);                 // альбомная ориентация, 320x240
  tft.fillScreen(ILI9341_BLACK);

  tft.fillRect(0, 0, 320, 26, ILI9341_NAVY);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(8, 5);
  tft.print(F("UNO + ILI9341"));
}

void loop() {
  static uint32_t cnt = 0;
  cnt++;

  tft.fillRect(8, 40, 220, 16, ILI9341_BLACK);
  tft.setTextColor(ILI9341_GREEN);
  tft.setCursor(8, 40);
  tft.print(F("Loop: "));
  tft.print(cnt);

  int v = analogRead(A0);
  tft.fillRect(8, 70, 304, 24, ILI9341_BLACK);
  tft.drawRect(8, 70, 304, 24, ILI9341_WHITE);
  tft.fillRect(10, 72, map(v, 0, 1023, 0, 300), 20, ILI9341_CYAN);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setCursor(8, 100);
  tft.print(F("A0 = "));
  tft.print(v);
  tft.print(F("     "));

  // бегущая синусоида: старая стирается, новая рисуется
  for (int x = 0; x < 320; x++) {
    int yOld = 170 + 40 * sin((x + (cnt - 1) * 4) * 0.02);
    tft.drawPixel(x, yOld, ILI9341_BLACK);
    int yNew = 170 + 40 * sin((x + cnt * 4) * 0.02);
    tft.drawPixel(x, yNew, ILI9341_RED);
  }

  delay(30);
}