#pragma once

#include <cstdint>
#include <vector>
#include <functional>
#include <set>

#include "Breakpoint.h"
#include "State.h"

class Emulator8080 {
public:
    Emulator8080();

    // initialise the emulator
    /// @todo should just provide memory implementation + pc to start from
    void init(uint8_t* rom, uint16_t romSize,  uint16_t pc, uint16_t inWorkSize, uint16_t inVideoSize, bool isRamMirrorEnabled, bool isRomWriteable);

	// CallbackIn - invoked for IN opcode
	typedef std::function<uint8_t(uint8_t port)> CallbackIn;
	void setCallbackIn(CallbackIn callback);

	// CallbackOut - invoked for OUT opcode
	typedef std::function<void(uint8_t port, uint8_t value)> CallbackOut;
	void setCallbackOut(CallbackOut callback);

    // step through an instruction at current address in pc
    void step();

    // get the number of steps that have been simulated so far
    uint64_t getNumSteps() const;      

    // get the current state of the CPU
    const State& getState() const;

    // get byte from memory
    /// @todo have private function for CPU that accesses public function on memory
    uint8_t readMemory(uint16_t address) const;      

	// get address at top of RAM
    /// @todo: move to memory.size() ?
	uint16_t getRamTop() const;

	// trigger interrupt on CPU
	void interrupt(int interruptNum);

	// get the current state of video ram
    /// @todo replace by using readMemory()
	const std::vector<uint8_t> getVideoRam() const;
    
    // add a breakpoint that is fired when an address is written to
    void addBreakpoint(const Breakpoint& breakpoint);

    // CallbackBreakpoint - optional callback fired when a breakpoint is reached
    
    typedef std::function<void(const Breakpoint& breakpoint, uint16_t value)> CallbackBreakpoint;
    void setCallbackBreakpoint(CallbackBreakpoint callback);
        
private:
    size_t unimplementedOpcode(uint16_t pc);
    uint16_t readOpcodeD16(uint8_t* opcode);
    void writeMemory(uint16_t address, uint8_t value);    
    void call(uint16_t address, uint16_t returnAddress);
    void ret();
    
    State           state;
    
    uint8_t*        rom;  
	uint16_t        romSize;

    std::vector<uint8_t> work;
    std::vector<uint8_t> video;

	uint16_t		workTop;
	uint16_t		videoTop;
	uint16_t		ramSize;

	bool			isRamMirrorEnabled;
	bool			isRomWriteable;

    uint64_t        numSteps;

	CallbackIn		callbackIn;
	CallbackOut		callbackOut;
    CallbackBreakpoint callbackBreakpoint;

    struct Breakpoints {
        std::set<uint16_t> memoryWrite;
        std::set<uint16_t> opcode;
    } breakpoints;
};
