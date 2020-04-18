#include "cpu/ConditionCodes.h"

namespace {
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
}

ConditionCodes::ConditionCodes() : all(0) {

}

void ConditionCodes::reset() {
	all = 0;
}

void ConditionCodes::updateByteZSP(uint16_t value) {
	z = ((value & 0xff) == 0);
	s = ((value & 0x80) == 0x80);
	p = parity(value & 0xff);

	// ac (auxilliary carry) not implemented - not required for space invaders
}

void ConditionCodes::updateByteCY(uint16_t value) {
	cy = (value > 0xff);
}

void ConditionCodes::updateWordCY(uint32_t value) {
	cy = (value > 0xffff);
}
