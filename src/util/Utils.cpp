#include "util/Utils.h"

#include <cassert>
#include <iostream>
#include <fstream>

namespace util {
	uint16_t makeWord(uint8_t hi, uint8_t lo) {
		return (uint16_t(hi) << 8) | uint16_t(lo);
	}

	void swap(uint8_t& a, uint8_t& b) {
		uint8_t temp = a;
		a = b;
		b = temp;
	}

	bool parity(uint16_t value) {
		int size = 16;
		int count = 0;

		for (int i = 0; i < size; i++) {
			if (value & 0x01) {
				count += 1;
			}

			value >>= 1;
		}

		return (0 == (count & 0x1));
	}

	size_t getFileSize(const char* filename) {
		std::ifstream file;
		file.open(filename, std::ios::in | std::ios::binary | std::ios::ate);
		assert(file.is_open());

		size_t size = static_cast<size_t>(file.tellg());

		file.close();

		return size;
	}
}
