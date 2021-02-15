#pragma once

#include <cstdint>
#include <cstddef>

namespace util {
	// make a 16bit word from two bytes
	uint16_t makeWord(uint8_t hi, uint8_t lo);

	// swap two bytes in place
	void swap(uint8_t& a, uint8_t& b);

	// return true if value has an even number of bits set to 1
	bool parity(uint16_t value);

	// measure size of file
	size_t getFileSize(const char* filename);
}