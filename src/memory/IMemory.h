#pragma once

#include <cstdint>

namespace memory {

	class IMemory {
	public:
		virtual ~IMemory() {};

		// apply any address translation required for this address
		virtual uint16_t translate(uint16_t address) const = 0;

		// write a byte to specified address
		virtual void write(uint16_t address, uint8_t value) = 0;

		// read a byte from specified address
		virtual uint8_t read(uint16_t address) const = 0;
	};
}
