#pragma once

#include <cstdint>

struct Breakpoint {
    enum class Type {
        MemoryWrite,
        Opcode
    };

    Breakpoint(Type type, uint16_t address);
    
    Type type;

    uint16_t address;
};
