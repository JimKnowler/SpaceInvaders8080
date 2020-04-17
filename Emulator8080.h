#pragma once

#include <cstdint>
#include <vector>
#include <functional>
#include <set>

class Emulator8080 {
public:
    Emulator8080();

    // initialise the emulator
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

    struct ConditionCodes {
        uint8_t    z : 1;
        uint8_t    s : 1;
        uint8_t    p : 1;
        uint8_t    cy : 1;
        uint8_t    ac : 1;
        uint8_t    pad : 3;
    };

    struct State {
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

    // get the current state of the CPU
    const State& getState() const;

    // get byte from memory
    uint8_t readMemory(uint16_t address) const;

    // get HL
    uint16_t getHL() const;

    // get DE
    uint16_t getDE() const;

    // get BC
    uint16_t getBC() const;

	// get address at top of RAM
	uint16_t getRamTop() const;

	// trigger interrupt on CPU
	void interrupt(int interruptNum);

	// get the current state of video ram
	const std::vector<uint8_t> getVideoRam() const;
    
    // add a breakpoint that is fired when an address is written to
    void addBreakpointMemoryWrite(uint16_t address);

    // CallbackBreakpoint - optional callback fired when a breakpoint is reached
    enum class BreakPoint {
        MemoryWrite
    };
    typedef std::function<void(BreakPoint type, uint16_t address, uint16_t value)> CallbackBreakpoint;
    void setCallbackBreakpoint(CallbackBreakpoint callback);
    
private:
    void updateZSP(uint16_t answer);
    void updateCY(uint16_t value);
    void updateWordCY(uint32_t value);
    size_t unimplementedOpcode(uint16_t pc);
    uint16_t readOpcodeD16(uint8_t* opcode);
    void writeMemory(uint16_t address, uint8_t value);
    void setDE(uint16_t value);
    void setHL(uint16_t value);
	void setBC(uint16_t value);
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

    std::set<uint16_t> breakpointsMemoryWrite;
};
