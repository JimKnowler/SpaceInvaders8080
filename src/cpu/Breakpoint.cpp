#include "cpu/Breakpoint.h"

namespace cpu {
	Breakpoint::Breakpoint(Type inType, uint16_t inAddress) : type(inType), address(inAddress) {

	}
}
