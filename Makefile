# MacOS makefile

LIBRARIES = -framework OpenGL -framework GLUT -lpng
HOMEBREW_INCLUDE = /opt/homebrew/include
HOMEBREW_LIB = /opt/homebrew/lib
EXE = SpaceInvaders8080

all:
	clang++ -std=c++17 -mmacosx-version-min=10.15 -Wall $(LIBRARIES) -Isrc -I${HOMEBREW_INCLUDE} -L${HOMEBREW_LIB} src/main.cpp \
	src/Disassemble.cpp src/cpu/Breakpoint.cpp src/cpu/ConditionCodes.cpp src/cpu/CPU.cpp src/cpu/Register16.cpp \
	src/cpu/State.cpp src/memory/Memory.cpp src/util/Utils.cpp \
	-o $(EXE)
clean:
	-rm $(EXE)