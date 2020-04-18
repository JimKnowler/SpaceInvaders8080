#include "util/Utils.h"

uint16_t makeWord(uint8_t hi, uint8_t lo) {
	return (uint16_t(hi) << 8) | uint16_t(lo);
}

void swap(uint8_t& a, uint8_t& b) {
	uint8_t temp = a;
	a = b;
	b = temp;
}