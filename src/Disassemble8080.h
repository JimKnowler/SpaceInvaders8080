#pragma once

#include <string>

namespace Disassemble8080 {
	uint16_t dissassembleOpcode(uint8_t* code, std::string& outOpcode);
}