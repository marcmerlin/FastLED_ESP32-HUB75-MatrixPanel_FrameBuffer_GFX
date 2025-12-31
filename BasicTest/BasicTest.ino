#include "matrix_config.h"

void setup() {
    Serial.begin(115200);
    delay(250);
    matrix_setup();
    // just FYI this is not needed for the ESP32 DMA lib, but other libs may need it.
    matrix->begin();
}

void loop() {
    Serial.println("Cycling red");
    matrix->fillScreen(0xf800);
    matrix->show();
    for (uint8_t b = 255; b>0; b--) {
        matrix->setBrightness(b);
        delay(1);
    }
    Serial.println("Dimmed to black");
    matrix->setBrightness(0);
    delay(1000);
    Serial.println("Cycling blue");
    matrix->fillScreen(0x001f);
    matrix->show();
    for (uint8_t b = 0; b < 255; b++) {
        matrix->setBrightness(b);
        delay(1);
    }
}
