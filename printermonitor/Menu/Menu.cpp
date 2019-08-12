#include "Menu.h"
#include "MenuFonts.h"

Menu::Menu()
{
    menuItems = LinkedList<MenuItem *>();
}

Menu::Menu(String menuTitle)
{
    MenuTitle = menuTitle;
    Menu();
}

void Menu::beforeShow() {
}

MenuItem* Menu::getItemForKey (char c) {
    if (c >= '1' && c <= '6') {
        int offset = c - '1';

        if (offset < MENU_MAX_DISPLAY_ITEMS) {
            int itemNum = startPage * MENU_MAX_DISPLAY_ITEMS + offset;

            if (itemNum < menuItems.size()) {
                return menuItems[itemNum];
            }
        }
    }

    return NULL;
}

void Menu::processKey(char c)
{
    switch (c)
    {
    case '0':
        doExit();
        break;
    case '*':
        if (startPage > 0)
        {
            startPage = startPage - 1;
        }
        break;
    case '#':
        if (startPage < getMaxPage())
        {
            startPage++;
        }
        break;
    }

    displayMillis = millis();
}

void Menu::doExit(void)
{
    if (exitCallback != NULL)
    {
        exitCallback();
    }
}
int Menu::getMaxPage()
{
    return ceil(menuItems.size() / MENU_MAX_DISPLAY_ITEMS);
}
void Menu::drawMenu(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
    if (displayMillis + MENU_TIMEOUT < millis())
    {
        doExit();
        return;
    }
    display->setColor(WHITE);
    display->setFont((const uint8_t *)ArialMT_Plain_10);
    display->setTextAlignment(TEXT_ALIGN_CENTER);

    display->fillRect(0, 0, 128, 11);

    display->setColor(BLACK);
    display->drawString(64, 0, MenuTitle);
    display->setColor(WHITE);

    display->setTextAlignment(TEXT_ALIGN_LEFT);

    bool hasPrevPage = startPage > 0;
    bool hasNextPage = true;
    int startIndex = startPage * MENU_MAX_DISPLAY_ITEMS;
    int endIndex = startIndex + MENU_MAX_DISPLAY_ITEMS;
    if (endIndex + startIndex > menuItems.size() - 1)
    {
        endIndex = menuItems.size();
        hasNextPage = false;
    }

    int xPos = 0;
    int yPos = MENU_ITEM_START_POS_Y;
    char startKey = '1';
    bool isSecondColumn = false;

    for (int i = startIndex; i < endIndex; i++)
    {
        MenuItem *menuItem = menuItems[i];

        drawRegularKeyboardButtonIcon(display, xPos, yPos, startKey, false, true);
        startKey++;

        display->setFont((const uint8_t *)DialogInput_plain_11);
        display->setTextAlignment(TEXT_ALIGN_LEFT);
        display->drawString(xPos + REGULAR_KEYPAD_BUTTON_ICON_WIDTH + 2, yPos - 1, menuItem->ItemName);
        if (!isSecondColumn)
        {
            display->setColor(BLACK);
            // Clear string remainder
            display->fillRect(xPos + MENU_ITEM_SECOND_COLUMN_START_X, yPos, xPos + MENU_ITEM_SECOND_COLUMN_START_X + MENU_ITEM_SECOND_COLUMN_START_X, yPos + MENU_ITEM_HEIGHT);
            display->setColor(WHITE);
        }

        yPos = yPos + MENU_ITEM_HEIGHT;

        if (floor(MENU_MAX_DISPLAY_ITEMS / 2) - 1 == i)
        {
            isSecondColumn = true;
            yPos = MENU_ITEM_START_POS_Y;
            xPos = MENU_ITEM_SECOND_COLUMN_START_X;
        }
    }

        display->fillRect(0, MENU_BOTTOM_BAR_LINE, 128, 64 - MENU_BOTTOM_BAR_LINE + 1);

        /* bottomBarStart = 44;
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->setColor(BLACK);
  display->drawString(SMALL_KEYPAD_BUTTON_ICON_WIDTH + 1, bottomBarStart - 2, "Enter manually");
  //display->drawString(44 + SMALL_KEYPAD_BUTTON_ICON_WIDTH+1, bottomBarStart-1, "Exit");
  //display->drawString(88 + SMALL_KEYPAD_BUTTON_ICON_WIDTH+1, bottomBarStart-1, "Next");

  drawSmallKeyboardButtonIcon(display, 1, bottomBarStart, '7', true, true);
  //drawSmallKeyboardButtonIcon(display, 44, bottomBarStart, '0');
  //drawSmallKeyboardButtonIcon(display, 88, bottomBarStart, '#');
*/
        int bottomBarStart = 54;

        if (hasNextPage)
        {
            display->setTextAlignment(TEXT_ALIGN_LEFT);
            display->setFont(ArialMT_Plain_10);
            display->setColor(BLACK);
            display->drawString(88 + SMALL_KEYPAD_BUTTON_ICON_WIDTH + 1, bottomBarStart - 2, "Next");
            drawSmallKeyboardButtonIcon(display, 89, bottomBarStart, '#', true, true);
        }

        if (hasPrevPage)
        {
            display->setTextAlignment(TEXT_ALIGN_LEFT);
            display->setFont(ArialMT_Plain_10);
            display->setColor(BLACK);
            display->drawString(SMALL_KEYPAD_BUTTON_ICON_WIDTH + 1, bottomBarStart - 2, "Prev");
            drawSmallKeyboardButtonIcon(display, 1, bottomBarStart, '*', true, true);
        }

        display->setTextAlignment(TEXT_ALIGN_LEFT);
        display->setFont(ArialMT_Plain_10);
        display->setColor(BLACK);
        display->drawString(44 + SMALL_KEYPAD_BUTTON_ICON_WIDTH + 1, bottomBarStart - 2, "Exit");

        drawSmallKeyboardButtonIcon(display, 45, bottomBarStart, '0', true, true);
    
}

void Menu::drawKeyboardButtonIcon(OLEDDisplay *display, int16_t x, int16_t y, char keyButton, const uint8_t *font, int16_t iconWidth, int16_t iconHeight, int16_t xOffset, int16_t yOffset, bool inverted, bool filled)
{
    OLEDDISPLAY_COLOR foregroundColor = WHITE;
    OLEDDISPLAY_COLOR backgroundColor = BLACK;

    if (inverted)
    {
        foregroundColor = BLACK;
        backgroundColor = WHITE;
    }

    display->setFont(font);
    display->setTextAlignment(TEXT_ALIGN_CENTER_BOTH);

    display->setColor(foregroundColor);
    if (filled)
    {

        display->fillRect(x, y, iconWidth, iconHeight);
        display->setColor(backgroundColor);
    }
    else
    {
        display->drawRect(x, y, iconWidth, iconHeight);
    }

    display->drawString(x + xOffset + iconWidth / 2, y + yOffset + iconHeight / 2, String(keyButton));

    display->setColor(WHITE);
}

void Menu::drawRegularKeyboardButtonIcon(OLEDDisplay *display, int16_t x, int16_t y, char keyButton, bool inverted, bool filled)
{
    drawKeyboardButtonIcon(display, x, y, keyButton, (const uint8_t *)Open_Sans_ExtraBold_14,
                           REGULAR_KEYPAD_BUTTON_ICON_WIDTH, REGULAR_KEYPAD_BUTTON_ICON_HEIGHT, REGULAR_KEYPAD_BUTTON_ICON_X_OFFSET, REGULAR_KEYPAD_BUTTON_ICON_Y_OFFSET, inverted, filled);
}

void Menu::drawSmallKeyboardButtonIcon(OLEDDisplay *display, int16_t x, int16_t y, char keyButton, bool inverted, bool filled)
{
    drawKeyboardButtonIcon(display, x, y, keyButton, (const uint8_t *)ArialMT_Plain_10,
                           SMALL_KEYPAD_BUTTON_ICON_WIDTH, SMALL_KEYPAD_BUTTON_ICON_HEIGHT, SMALL_KEYPAD_BUTTON_ICON_X_OFFSET, SMALL_KEYPAD_BUTTON_ICON_Y_OFFSET, inverted, filled);
}