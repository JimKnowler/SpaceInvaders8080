#pragma once

#include <cstdint>

#include "Register16.h"

struct State {
    State();

    struct ConditionCodes {
        uint8_t    z : 1;
        uint8_t    s : 1;
        uint8_t    p : 1;
        uint8_t    cy : 1;
        uint8_t    ac : 1;
        uint8_t    pad : 3;
    };
    
    Register16 hl;
    Register16 de;
    Register16 bc;

    uint8_t     a;
    uint8_t     b;
    uint8_t     c;
    uint8_t     d;
    uint8_t     e;
    uint8_t     h;
    uint8_t     l;
    uint16_t    sp;
    uint16_t    pc;

    ConditionCodes cc;

    bool interruptsEnabled;
};