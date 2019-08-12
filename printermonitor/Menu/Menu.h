#pragma once

#include <Arduino.h>
#include "MenuItem.h"
#include <LinkedList.h>
#include <OLEDDisplay.h>
#include <OLEDDisplayUi.h>

#define MENU_TIMEOUT 5000

#define MENU_MAX_DISPLAY_ITEMS 4
#define MENU_ITEM_START_POS_Y 14
#define MENU_ITEM_SECOND_COLUMN_START_X 64
#define MENU_ITEM_HEIGHT 14

#define MENU_BOTTOM_BAR_LINE 43

#define REGULAR_KEYPAD_BUTTON_ICON_WIDTH 12
#define REGULAR_KEYPAD_BUTTON_ICON_HEIGHT 12
#define REGULAR_KEYPAD_BUTTON_ICON_X_OFFSET 0
#define REGULAR_KEYPAD_BUTTON_ICON_Y_OFFSET 0

#define SMALL_KEYPAD_BUTTON_ICON_WIDTH 7
#define SMALL_KEYPAD_BUTTON_ICON_HEIGHT 9
#define SMALL_KEYPAD_BUTTON_ICON_X_OFFSET 1
#define SMALL_KEYPAD_BUTTON_ICON_Y_OFFSET 0

class Menu
{
public:
    Menu();
    Menu(String menuTitle);
    String MenuTitle;
    void drawMenu(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
    virtual void beforeShow();
    virtual void processKey(char c);
    MenuItem* getItemForKey (char c);
    int getMaxPage();
    void (*exitCallback)(void) = NULL;
    int startPage = 0;
    LinkedList<MenuItem *> menuItems;
    unsigned long displayMillis;

    protected:
    void doExit (void);
private:

    void drawRegularKeyboardButtonIcon(OLEDDisplay *display, int16_t x, int16_t y, char keyButton, bool inverted, bool filled);
    void drawSmallKeyboardButtonIcon(OLEDDisplay *display, int16_t x, int16_t y, char keyButton, bool inverted, bool filled);
    void drawKeyboardButtonIcon(OLEDDisplay *display, int16_t x, int16_t y, char keyButton, const uint8_t *font, int16_t iconWidth, int16_t iconHeight, int16_t xOffset, int16_t yOffset, bool inverted, bool filled);
};