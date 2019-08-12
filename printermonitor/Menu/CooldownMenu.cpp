#include "CooldownMenu.h"
#include "TemperatureMenuItem.h"
#include "TemperaturePreset.h"

CooldownMenu::CooldownMenu()
{
    MenuTitle = "Cooldown Menu";
}

void CooldownMenu::processKey(char c)
{

    if (c >= '1' && c <= '6')
    {

        TemperatureMenuItem *item = (TemperatureMenuItem *)getItemForKey(c);

        if (item != NULL)
        {
            TriggerCooldown(item);
            doExit();
        }
    }

    Menu::processKey(c);
}

void CooldownMenu::TriggerCooldown(TemperatureMenuItem *item)
{
    if (item->preheatBed)
    {
        client->preheatBed(item->preset->bed);
    }

    if (item->preheatChamber)
    {
        client->preheatChamber(item->preset->chamber);
    }

    if (item->preheatExtruder)
    {
        client->preheatTool(0, item->preset->extruder);
    }
}

void CooldownMenu::beforeShow()
{
    TemperaturePreset *cooldown = new TemperaturePreset();
    cooldown->bed = 0;
    cooldown->chamber = 0;
    cooldown->extruder = 0;

    menuItems.clear();
    TemperatureMenuItem *all = new TemperatureMenuItem("All");
    all->preset = cooldown;

    all->preheatBed = true;
    all->preheatChamber = true;
    all->preheatExtruder = true;

    menuItems.add(all);

    TemperatureMenuItem *extruder = new TemperatureMenuItem("Extruder");
    extruder->preset = cooldown;
    extruder->preheatExtruder = true;

    menuItems.add(extruder);

    TemperatureMenuItem *bed = new TemperatureMenuItem("Bed");
    bed->preset = cooldown;
    bed->preheatBed = true;

    menuItems.add(bed);

    TemperatureMenuItem *chamber = new TemperatureMenuItem("Chamber");
    chamber->preset = cooldown;
    chamber->preheatChamber = true;

    menuItems.add(chamber);
}
