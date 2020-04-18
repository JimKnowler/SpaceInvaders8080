#pragma once

#include <cstdint>

struct State {
    struct ConditionCodes {
        uint8_t    z : 1;
        uint8_t    s : 1;
        uint8_t    p : 1;
        uint8_t    cy : 1;
        uint8_t    ac : 1;
        uint8_t    pad : 3;
    };

    uint16_t getHL() const;
    uint16_t getDE() const;
    uint16_t getBC() const;

    void setDE(uint16_t value);
    void setHL(uint16_t value);
    void setBC(uint16_t value);

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