# MacOS makefile

LIBRARIES = -framework OpenGL -framework GLUT -lpng

EXE = SpaceInvaders8080

all:
	clang++ -arch x86_64 -std=c++17 -mmacosx-version-min=10.15 -Wall $(LIBRARIES) -Isrc src/main.cpp \
	src/Disassemble.cpp src/cpu/Breakpoint.cpp src/cpu/ConditionCodes.cpp src/cpu/CPU.cpp src/cpu/Register16.cpp \
	src/cpu/State.cpp src/memory/Memory.cpp src/util/Utils.cpp \
	-o $(EXE)
clean:
	-rm $(EXE)