#pragma once

#include "Menu.h"
#include "../OctoPrintClient.h"
#include <LinkedList.h>
#include "TemperatureMenuItem.h"

class PreheatMenu : public Menu
{
public:
    PreheatMenu();
    void beforeShow() override;
    void processKey(char c) override;
    void RetrieveTemperatures();
    void CreateTemperatureMenu (TemperatureMenuItem* item);
    void TriggerPreheat (TemperatureMenuItem* item);
    LinkedList<MenuItem *> presetMenuItems;
    LinkedList<MenuItem *> temperatureItems;
    OctoPrintClient *client;
    private:
    bool selectPreset = true;
};