#pragma once

#include <cstdint>

namespace cpu {
    struct Register16 {
        Register16(uint8_t& hi, uint8_t& lo);

        // assignment
        Register16& operator=(uint16_t value);

        // retrieval
        operator uint16_t() const;

        uint8_t& hi;
        uint8_t& lo;
    };
}
