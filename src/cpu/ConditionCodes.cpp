#include "cpu/ConditionCodes.h"
#include "util/Utils.h"

namespace cpu {

	ConditionCodes::ConditionCodes() : all(0) {

	}

	void ConditionCodes::reset() {
		all = 0;
	}

	void ConditionCodes::updateByteZSP(uint16_t value) {
		z = ((value & 0xff) == 0);
		s = ((value & 0x80) == 0x80);
		p = util::parity(value & 0xff);

		// ac (auxilliary carry) not implemented - not required for space invaders
	}

	void ConditionCodes::updateByteCY(uint16_t value) {
		cy = (value > 0xff);
	}

	void ConditionCodes::updateWordCY(uint32_t value) {
		cy = (value > 0xffff);
	}

}