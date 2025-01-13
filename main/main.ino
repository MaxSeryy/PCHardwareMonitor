//This code is a very simplified version for 1.8 TFT 128x160 screen of the original code
//The original code ---> PCdisplay by AlexGyver ---> https://github.com/AlexGyver/PCdisplay
//Use the Libre Hardware Monitor 05.2022 version to get the data from the PC

//0-CPU temp, 1-GPU temp, 2-mother temp, 3-max HDD temp, 4-CPU load, 5-GPU load, 6-RAM use, 7-GPU memory use
//And a lot more data for the original project

#include <SPI.h>
#include <TFT.h>

#define cs 10
#define dc 9
#define rst 8

TFT TFTscreen = TFT(cs, dc, rst);

char inData[82];
int PCdata[20];
byte index = 0;
boolean updateDisplay_flag = 0;

void setup() {
    Serial.begin(9600);
    TFTscreen.begin();
    TFTscreen.background(0, 0, 0);
    TFTscreen.setTextSize(2);
    TFTscreen.stroke(255, 255, 255);
    TFTscreen.text("CPU ", 0, 0);
    TFTscreen.text("CPU ", 0, 20);
    TFTscreen.text("GPU ", 0, 40);
    TFTscreen.text("GPU ", 0, 60);
    TFTscreen.text("RAM ", 0, 80);
    TFTscreen.text("HDD ", 0, 100);
}

void loop() {
    parsing();
    updateDisplay();
}

void parsing() {
    while (Serial.available() > 0) {
        char aChar = Serial.read();
        if (aChar != 'E') {
            inData[index] = aChar;
            index++;
            inData[index] = '\0';
        } else {
            char *p = inData;
            char *str;
            index = 0;
            while ((str = strtok_r(p, ";", &p)) != NULL) {
                PCdata[index] = atoi(str);
                index++;
            }
            index = 0;
            updateDisplay_flag = 1;
        }
    }
}

int prevData[20] = {0};

void drawDegreeSymbol(int x, int y) { TFTscreen.circle(x, y, 2); } // °

void setColorBasedOnValue(int value, bool isTemp) {
    int red = 255;
    int green = 255;
    int blue = 255;

    if (value > 45 && value <= 90) {
        red = map(value, 45, 90, 255, 255);
        green = map(value, 45, 90, 255, 0);
        blue = map(value, 45, 90, 0, 0);
    } else if (value > 90) {
        red = 255;
        green = 0;
        blue = 0;
    }

    TFTscreen.stroke(blue, green, red); // BGR - IDK why...
}
void drawBars(int value, int maxValue, int x, int y, bool isTemp) { 
    int barCount = map(value, 0, maxValue, 0, 12); // 12 bars
    for (int i = 0; i < barCount; i++) {
        setColorBasedOnValue(value, isTemp);
        TFTscreen.text("|", x + i * 6, y); // 6 px per bar
    }
}

void updateDisplay() {
    if (updateDisplay_flag) {
        for (int i = 0; i < 20; i++) {
            if (PCdata[i] != prevData[i]) {
                switch (i) {
                    case 0:
                        TFTscreen.fillRect(40, 0, 160, 15, 0);
                        setColorBasedOnValue(PCdata[0], true);
                        TFTscreen.text((String(PCdata[0]) + " ").c_str(), 40, 0);
                        drawDegreeSymbol(40 + String(PCdata[0]).length() * 13, 3);
                        drawBars(PCdata[0], 100, 85, 0, true);
                        break;
                    case 4:
                        TFTscreen.fillRect(40, 20, 160, 15, 0);
                        setColorBasedOnValue(PCdata[4], true);
                        TFTscreen.text((String(PCdata[4]) + "%").c_str(), 40, 20);
                        drawBars(PCdata[4], 100, 85, 20, true);
                        break;
                    case 1:
                        TFTscreen.fillRect(40, 40, 160, 15, 0);
                        setColorBasedOnValue(PCdata[1], true);
                        TFTscreen.text((String(PCdata[1]) + " ").c_str(), 40, 40);
                        drawDegreeSymbol(40 + String(PCdata[1]).length() * 13, 43);
                        drawBars(PCdata[1], 100, 85, 40, true);
                        break;
                    case 5:
                        TFTscreen.fillRect(40, 60, 160, 15, 0);
                        setColorBasedOnValue(PCdata[5], true);
                        TFTscreen.text((String(PCdata[5]) + "%").c_str(), 40, 60);
                        drawBars(PCdata[5], 100, 85, 60, true);
                        break;
                    case 6:
                        TFTscreen.fillRect(40, 80, 160, 15, 0);
                        setColorBasedOnValue(PCdata[6], true);
                        TFTscreen.text((String(PCdata[6]) + "%").c_str(), 40, 80);
                        drawBars(PCdata[6], 100, 85, 80, true);
                        break;
                    case 3:
                        TFTscreen.fillRect(40, 100, 160, 15, 0);
                        setColorBasedOnValue(PCdata[3], true);
                        TFTscreen.text((String(PCdata[3])).c_str(), 40, 100);
                        drawDegreeSymbol(40 + String(PCdata[3]).length() * 13, 103);
                        drawBars(PCdata[3], 100, 85, 100, true);
                        break;
                }
            }
        }
        memcpy(prevData, PCdata, sizeof(PCdata));
        updateDisplay_flag = 0;
    }
}