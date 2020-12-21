#include "EspController.h"

/**
 * @brief Get current WiFi Quality in percent
 * 
 * @return int8_t 
 */
int8_t EspController::getWifiQuality() {
    int32_t dbm = WiFi.RSSI();
    if(dbm <= -100) {
        return 0;
    } else if(dbm >= -50) {
        return 100;
    }
    return 2 * (dbm + 100);
}

/**
 * @brief Get heap memory stats for the esp
 * 
 * @param hfree     Free heap
 * @param hmax      Max continues blocks reserverd
 * @param hfrag     Heap fragmentation in percent
 */
void EspController::getHeap(uint32_t* hfree, uint16_t* hmax, uint8_t* hfrag) {
    ESP.getHeapStats(hfree, hmax, hfrag);
}
