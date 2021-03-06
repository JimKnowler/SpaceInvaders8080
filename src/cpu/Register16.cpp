#include "cpu/Register16.h"
#include "util/Utils.h"

namespace cpu {
	Register16::Register16(uint8_t& _hi, uint8_t& _lo) : hi(_hi), lo(_lo) {

	}

	Register16& Register16::operator=(uint16_t value) {
		hi = uint8_t((value >> 8) & 0xff);
		lo = uint8_t(value & 0xff);

		return *this;
	}

	Register16::operator uint16_t() const {
		uint16_t value = util::makeWord(hi, lo);

		return value;
	}
}