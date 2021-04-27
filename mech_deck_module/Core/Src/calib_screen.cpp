//
// Created by Ilia.Motornyi on 16/06/2019.
//

#include <deck-module.hpp>

#define GRAPH_WIDTH 120
#define GRAPH_HEIGHT 120
#define TICK_STEPS 5
#define ZOOM_POSITION (LAST_STD_BUTTON + 1)


CalibScreen calibScreen;
static char strBuf[SCREEN_WIDTH_BYTES + 1];

//todo help screen?
//todo fix reboot
//todo nmea retranslator

void CalibScreen::updatePicture() {
    Display::copyPict(myBackground);
    float wind = calcSpeed(localWindTable, state.windTics);
    if (state.windTics > 0) {
        myAffineTransform.line(0, wind, (float) state.windTics, wind, 1, "100");
        myAffineTransform.line((float) state.windTics, 0, (float) state.windTics, wind, 1, "100");
        snprintf(strBuf, sizeof(strBuf), "%04d %4.1fm/s", (int) state.windTics, wind);
        Display::writeTextLine(strBuf, 16, 0);
    } else {
        Display::writeTextLine("------------", 16, 0);
    }
    int cursorX = 0;
    int cursorY;
    //"Save Close Next Zoom"
    if (position <= LAST_STD_BUTTON + 1) {
        cursorY = 15;
        switch (position) {
            case SAVE_POSITION:
                cursorX = 0;
                break;
            case CLOSE_POSITION:
                cursorX = 5;
                break;
            case NEXT_POSITION:
                cursorX = 11;
                break;
            case ZOOM_POSITION:
                cursorX = 16;
                break;
            default:;
        }
    } else {
        int tablePosition = position - ZOOM_POSITION - 1;
        cursorY = tablePosition / 8;
        cursorX = tablePosition % 8;
        switch (cursorX) {
            case 7:
                cursorX += 2;
                break;
            case 5:
            case 6:
                cursorX += 1;
                break;
            default:;
        }
    }
    Display::setCursor(cursorX, cursorY);
}

void CalibScreen::prepareBackground() {
    normalizetWindTable(localWindTable, &localSettings);
    float maxTps = localWindTable[0][0];
    int lastIndex = WIND_TABLE_LEN - 1;
    double maxX = -1;
    for (int i = 1; i < WIND_TABLE_LEN; i++) {
        if (localWindTable[i][0] == 0) {
            lastIndex = i - 1;
            break;
        }
        if (localWindTable[i][0] > maxTps) {
            maxTps = localWindTable[i][0];
        }
    }
    if (lastIndex >= 0) {
        double a = (localWindTable[lastIndex][1] - localWindTable[lastIndex - 1][1]) /
                   (localWindTable[lastIndex][0] - localWindTable[lastIndex - 1][0]);
        maxX = localWindTable[lastIndex][0] + ((float)maxScreenWind() - localWindTable[lastIndex][1]) / a;
    }
    if (maxX > 10000 || maxX <= 0) {
        maxX = maxTps;
    }
    Display::clearPict();

    const float cornerX = SCREEN_WIDTH - GRAPH_WIDTH;
    const float cornerY = SCREEN_HEIGHT - GRAPH_HEIGHT;
    const float stepX = GRAPH_WIDTH / (float) TICK_STEPS;
    const float stepY = GRAPH_HEIGHT / (float) TICK_STEPS;

    calibDisplay.line(cornerX, cornerY, cornerX, SCREEN_HEIGHT, 1, "1");
    calibDisplay.line(cornerX, cornerY, SCREEN_WIDTH, cornerY, 1, "1");

    for (int i = 1; i < TICK_STEPS; i++) {
        auto fI = (float) i;
        calibDisplay.line(cornerX, fI * stepY + cornerY, cornerX + 4, fI * stepY + cornerY, 1, "1");
        calibDisplay.line(cornerX + fI * stepX, cornerY, cornerX + fI * stepX, cornerY + 4, 1, "1");
    }
    myAffineTransform.reset();
    myAffineTransform.translate(cornerX, SCREEN_HEIGHT - GRAPH_HEIGHT);
    myAffineTransform.scale(GRAPH_WIDTH / (float) maxX, GRAPH_HEIGHT / (float) maxScreenWind());
    float x = 0;
    float y = 0;
    for (auto windTpsToM : localWindTable) {
        float t = windTpsToM[0];
        float v = windTpsToM[1];
        if (v == 0) break;
        if (x == t) continue;
        myAffineTransform.line(x, y, t, v, 1, "1");
        x = t;
        y = v;
    }
    float xLimit = zoom ? (float) maxX * 2 / 3 : (float) maxX;
    myAffineTransform.line(x, y, xLimit, (float) maxScreenWind(), 1, "10");

    Display::savePictTo(myBackground);
    printCalibrations();
    Display::writeTextLine("Save Close Next Zoom", 0, 15);
}

void CalibScreen::enter() {
    SettingsScreen::enter();
    prepareBackground();

    Display::setMode(true);
}

void CalibScreen::printCalibrations() {
    Display::clearText();
    Display::writeTextLine("0", 14, 14);
    itoa((int) maxScreenWind(), strBuf, 10);
    Display::writeTextLine(strBuf, 13, 0);
    for (int i = 0; i < WIND_TABLE_LEN; ++i) {
        snprintf(strBuf, sizeof(strBuf), "%c%04d %04.1f",
                 localSettings.windLineEnabled[i] ? '*' : '.',
                 (int) localSettings.windTpMtoMs[i][0],
                 localSettings.windTpMtoMs[i][1]);
        Display::writeTextLine(strBuf, 0, i);
    }
}

void CalibScreen::leave() {
    Screen::leave();
    Display::setMode(false);
}

int CalibScreen::maxPosition() {
    return LAST_STD_BUTTON + WIND_TABLE_LEN * 8;
}

static void increaseValue(float &value, int delta) {
    int delta10 = ((int) value / delta % 10);
    if (delta10 == 9) {
        value -= 9 * (float) delta;
    } else {
        value += (float) delta;
    }
}

void CalibScreen::changeValue(int delta) {
    if (position == ZOOM_POSITION) {
        zoom = !zoom;
    } else {
        int tablePosition = position - ZOOM_POSITION - 1;
        int cursorY = tablePosition / 8;
        int cursorX = tablePosition % 8;
        switch (cursorX) {
            case 0:
                localSettings.windLineEnabled[cursorY] ^= 1u;
                break;
            case 1:
                increaseValue(localSettings.windTpMtoMs[cursorY][0], 1000);
                break;
            case 2:
                increaseValue(localSettings.windTpMtoMs[cursorY][0], 100);
                break;
            case 3:
                increaseValue(localSettings.windTpMtoMs[cursorY][0], 10);
                break;
            case 4:
                increaseValue(localSettings.windTpMtoMs[cursorY][0], 1);
                break;
            case 5:
                increaseValue(localSettings.windTpMtoMs[cursorY][1], 10);
                break;
            case 6:
                increaseValue(localSettings.windTpMtoMs[cursorY][1], 1);
                break;
            case 7:
                localSettings.windTpMtoMs[cursorY][1] += 0.1;
                break;
            default:
                break;
        }
    }
    prepareBackground();
}

bool CalibScreen::isChanged() {
    return true;
}


