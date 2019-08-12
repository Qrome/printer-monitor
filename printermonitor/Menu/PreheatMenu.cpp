#include "PreheatMenu.h"
#include "TemperatureMenuItem.h"

PreheatMenu::PreheatMenu()
{
    MenuTitle = "Preheat Menu";
    presetMenuItems = LinkedList<MenuItem *>();
    temperatureItems = LinkedList<MenuItem *>();
}

void PreheatMenu::RetrieveTemperatures()
{
    presetMenuItems.clear();
    Serial.println(client->temperaturePresets.size());
    for (int i = 0; i < client->temperaturePresets.size(); i++)
    {
        TemperatureMenuItem *item = new TemperatureMenuItem(client->temperaturePresets[i]->PresetName);
        item->preset = client->temperaturePresets[i];

        presetMenuItems.add(item);
    }
}

void PreheatMenu::processKey(char c)
{

    if (c >= '1' && c <= '6')
    {
        if (selectPreset)
        {
            TemperatureMenuItem *item = (TemperatureMenuItem *)getItemForKey(c);

            if (item != NULL)
            {
                CreateTemperatureMenu(item);
                menuItems = temperatureItems;
                MenuTitle = "Preheat: " + item->ItemName;
                selectPreset = false;
            }
        }
        else
        {
            TemperatureMenuItem *item = (TemperatureMenuItem *)getItemForKey(c);

            if (item != NULL)
            {
                TriggerPreheat(item);
                doExit();
            }
        }
    }

    Menu::processKey(c);
}

void PreheatMenu::TriggerPreheat(TemperatureMenuItem *item) {
    if (item->preheatBed) {
        client->preheatBed(item->preset->bed);

    }

    if (item->preheatChamber) {
        client->preheatChamber(item->preset->chamber);
    }

    if (item->preheatExtruder) {
        client->preheatTool(0, item->preset->extruder);
    }
    
}

void PreheatMenu::CreateTemperatureMenu(TemperatureMenuItem *item)
{
    temperatureItems.clear();
    TemperatureMenuItem *all = new TemperatureMenuItem("All");
    all->preset = item->preset;
    all->preheatBed = true;
    all->preheatChamber = true;
    all->preheatExtruder = true;

    temperatureItems.add(all);

    if (item->preset->extruder > 0)
    {
        TemperatureMenuItem *extruder = new TemperatureMenuItem("Extruder");
        extruder->preset = item->preset;
        extruder->preheatExtruder = true;

        temperatureItems.add(extruder);
    }
    if (item->preset->bed > 0)
    {
        TemperatureMenuItem *bed = new TemperatureMenuItem("Bed");
        bed->preset = item->preset;
        bed->preheatBed = true;

        temperatureItems.add(bed);
    }

    if (item->preset->chamber > 0)
    {
        TemperatureMenuItem *chamber = new TemperatureMenuItem("Chamber");
        chamber->preset = item->preset;
        chamber->preheatChamber = true;

        temperatureItems.add(chamber);
    }
}

void PreheatMenu::beforeShow()
{
    RetrieveTemperatures();
    selectPreset = true;
    MenuTitle = "Preheat Menu";
    menuItems = presetMenuItems;
}