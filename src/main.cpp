#include <Arduino.h>
#include <LiquidCrystal.h>

// LCD pins: rs=12, en=14, d4=33, d5=27, d6=26, d7=25
LiquidCrystal lcd(12, 14, 33, 27, 26, 25);

String message = " Welcome to Yakha Soft";
int len;

void setup() {
    lcd.begin(16, 2);
    lcd.clear();
    len = message.length();
}

void loop() {

    // Scroll text left → right
    for (int i = 0; i < len; i++) {
        lcd.clear();

        // Extract 16 characters starting from i (circular scrolling)
        String window = "";
        for (int j = 0; j < 16; j++) {
            window += message[(i + j) % len];
        }

        lcd.setCursor(0, 0);
        lcd.print(window);

        delay(200); // smooth speed
    }
}
