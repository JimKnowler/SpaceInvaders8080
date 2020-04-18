#pragma once

#include <cstdint>

struct ConditionCodes {
    ConditionCodes();

    void reset();

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
