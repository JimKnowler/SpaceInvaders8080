# Space Invaders Arcade Emulator

Emulate `Intel 8080` CPU + ` Taito Space Invaders` machine Hardware.

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

By default, the emulator will run the original `Taito Space Invaders ROM`.

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

# Debugging Tools

The CPU emulation supports setting breakpoints for:

- Executing Opcode at a specified address
- Writing to memory at a specified address

Multiple breakpoints can be instantiated.

# Status
## Progress

- Player is able to insert coins + start a game
- Player is able to move left/right + fire
- Aliens are able to animate, move left/right + fire
- Player score is incrementing, hi-scores are recorded

## Issues

- Player + alien horizontal movements appear incorrect
  - unable to shift left/right by less than a byte?
  - leaving a trail of corrupted pixels behind after moving
    - this is affecting collision detection, which appears to be implemented on pixel values
  - perhaps start by looking at the external shift register for issues
- CPU emulation does not count cycles
  - Not currently required for Space Invaders simulation

## More Debugging Ideas

- support time travel debugging - the ability to roll forward/backwards in time through CPU + memory state
- support hardware breakpoints
  - when using OUT/IN on specific port, with specific value of A register

## Optimisation Ideas

- convert opcode switch to jump tables for O(1) execution
