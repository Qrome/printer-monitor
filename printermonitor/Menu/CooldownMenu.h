#pragma once

#include "Menu.h"
#include "../OctoPrintClient.h"
#include <LinkedList.h>
#include "TemperatureMenuItem.h"

class CooldownMenu : public Menu
{
public:
    CooldownMenu();
    void beforeShow() override;
    void processKey(char c) override;
    void TriggerCooldown (TemperatureMenuItem* item);
  
    OctoPrintClient *client;
};