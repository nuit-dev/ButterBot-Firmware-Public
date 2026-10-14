#include "Bonds.h"
#include <cstring>
#include <vector>

bool BLE::isBonded(const esp_bd_addr_t addr){
	int count = esp_ble_get_bond_device_num();
	if(count <= 0) return false;

	std::vector<esp_ble_bond_dev_t> list(count);
	if(esp_ble_get_bond_device_list(&count, list.data()) != ESP_OK) return false;

	for(int i = 0; i < count; i++){
		if(memcmp(list[i].bd_addr, addr, ESP_BD_ADDR_LEN) == 0) return true;
	}

	return false;
}
