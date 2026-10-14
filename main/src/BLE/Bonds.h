#ifndef BUTTERBOT_FIRMWARE_BONDS_H
#define BUTTERBOT_FIRMWARE_BONDS_H

#include <esp_gap_ble_api.h>

namespace BLE {

// Custom (NUIT): Bonds.h / Bonds.cpp are missing from the upstream 1.0.5 commit; ConMan and Client only need this.
// True if the address is in the bond list (Bluedroid reports a resolved peer under its identity address).
bool isBonded(const esp_bd_addr_t addr);

}

#endif //BUTTERBOT_FIRMWARE_BONDS_H
