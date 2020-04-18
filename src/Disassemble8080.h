#pragma once

#include <string>

#include "memory/IMemory.h"

namespace Disassemble8080 {
	uint16_t dissassembleOpcode(memory::IMemory* memory, uint16_t pc, std::string& outOpcode);
}