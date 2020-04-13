# Space Invaders Emulator

Emulate Intel 8080 CPU + Taito Space Invaders machine Hardware.

## Requirements

- Windows 10

- Visual Studio 2019 (tested with free 'community' edition)
  https://visualstudio.microsoft.com/downloads/

## References

- Using the OLC Pixel Game Engine for 2D UI
  https://github.com/OneLoneCoder/olcPixelGameEngine

- Implemented while following 8080 guide on emulator101
  http://emulator101.com/

- Information on SpaceInvaders hardware
  http://computerarcheology.com/Arcade/SpaceInvaders/Hardware.html

- 8080 Chipset opcodes
  http://www.emulator101.com/reference/8080-by-opcode.html

## CPU Diagnostics

To run the 8080 cpu diagnostic tests, enable the macro '`CPUDIAG`' in `BuildOptions.h`.

When the test is successful you should see a message in console TTY saying "`CPU IS OPERATIONAL`".

## Space Invaders

By default, the emulator will run the Taito Space Invaders ROM

## Instructions

The emulator starts in Debugger mode where you can step through opcodes, executing them one at a time.

The emulator can be switched into Run mode, where it will simulate running a Taito Space Invaders machine.

| Key |  Purpose |
|---|---|
| R  | Switch to `RUN` mode  |
| D  | Switch to `DEBUGGER` mode  |

### DEBUGGER Mode

| Key |  Purpose |
|---|---|
| SPACE  | Step through 1 opcode  |
| SHIFT+SPACE  | Step through 10 opcodes  |
| CTRL+SHIFT+SPACE  | Step through 100 opcodes  |

### RUN Mode

| Key |  Purpose |
|---|---|
| C  | Coin  |
| 1  | Start 1 player  |
| 2  | Start 2 player  |
| LEFT CURSOR  | Player 1 - Move Left  |
| RIGHT CURSOR  | Player 1 - Move Right  |
| SPACE  |  Player 1 - Fire |
| Z  | Player 2 - Move Left  |
| X  | Player 2 - Move Right  |
| SHIFT  | Player 2 - Fire  |


# TODO

Currently working on debugging the 'attract mode'.

- The external shift register is only being used with shift offset of 0
- the COIN input is not incrementing CREDITS

# DEBUGGING IDEAS

- support time travel debugging - the ability to roll forward/backwards in time through CPU + memory state
- keep track of opcode history so that we can trace back through recent opcode execution
- support hardware breakpoints
  - read/write memory addresses
  - when executing opcode at specific address
  - when using OUT/IN on specific port, with specific value of A register

