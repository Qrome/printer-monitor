#include "MenuItem.h"

class TemperatureMenuItem : public MenuItem
{
public:
    TemperatureMenuItem(String itemName);
    int bed;
    int chamber;
    int extruder;
    bool has_bed_temp;
    bool has_extruder_temp;
    bool has_chamber_temp;
};