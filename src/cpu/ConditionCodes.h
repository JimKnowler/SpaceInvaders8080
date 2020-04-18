#pragma once

#include <cstdint>

namespace cpu {

    struct ConditionCodes {
        ConditionCodes();

        void reset();

        void updateByteZSP(uint16_t value);
        void updateByteCY(uint16_t value);
        void updateWordCY(uint32_t value);

        union {
            struct {
                uint8_t    z : 1;
                uint8_t    s : 1;
                uint8_t    p : 1;
                uint8_t    cy : 1;
                uint8_t    ac : 1;
                uint8_t    pad : 3;
            };
            uint8_t all;
        };
    };

}