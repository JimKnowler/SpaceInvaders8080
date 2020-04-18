#pragma once

#include <cstdint>
#include <functional>
#include <set>

#include "cpu/Breakpoint.h"
#include "cpu/State.h"
#include "memory/IMemory.h"

namespace cpu {
    class CPU {
    public:
        CPU();

        // initialise the emulator
        void init(memory::IMemory* memory, uint16_t pcStart);

        // CallbackIn - invoked for IN opcode
        typedef std::function<uint8_t(uint8_t port)> CallbackIn;
        void setCallbackIn(CallbackIn callback);

        // CallbackOut - invoked for OUT opcode
        typedef std::function<void(uint8_t port, uint8_t value)> CallbackOut;
        void setCallbackOut(CallbackOut callback);

        // CallbackBreakpoint - invoked when a breakpoint is reached
        typedef std::function<void(const Breakpoint & breakpoint, uint16_t value)> CallbackBreakpoint;
        void setCallbackBreakpoint(CallbackBreakpoint callback);

        // step through an instruction at current address in pc
        void step();

        // trigger interrupt on CPU
        void interrupt(int interruptNum);

        // get the number of steps that have been simulated so far
        uint64_t getNumSteps() const;

        // get the current state of the CPU
        const State& getState() const;        

        // add a breakpoint that is fired when an address is written to
        void addBreakpoint(const Breakpoint& breakpoint);

    private:
        uint16_t unimplementedOpcode(uint16_t pc);
        uint16_t readOpcodeDataWord() const;
        uint8_t readMemory(uint16_t address) const;
        void writeMemory(uint16_t address, uint8_t value);
        void call(uint16_t address, uint16_t returnAddress);
        void ret();

        State state;

        memory::IMemory* memory;
        
        uint64_t numSteps;

        struct Callbacks {
            CallbackIn in;
            CallbackOut out;
            CallbackBreakpoint breakpoint;
        } callbacks;

        struct Breakpoints {
            std::set<uint16_t> memoryWrite;
            std::set<uint16_t> opcode;
        } breakpoints;
    };

}
