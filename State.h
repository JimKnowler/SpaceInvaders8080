#pragma once

#include <cstdint>

#include "ConditionCodes.h"
#include "Register16.h"

struct State {
    State();

    void reset();
        
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