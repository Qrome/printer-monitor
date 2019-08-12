#pragma once

#include "MenuItem.h"
#include "TemperaturePreset.h"

class TemperatureMenuItem : public MenuItem
{
public:
    TemperatureMenuItem(String itemName);
    TemperaturePreset* preset;
    bool preheatBed = false;
    bool preheatChamber = false;
    bool preheatExtruder = false;
};