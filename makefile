LDFLAGS = -lGL -lGLU -lglut
CFLAGS=-g -Wall -std=c++11 -Wdeprecated-declarations
CC=g++
EXEEXT=
RM=rm

ifeq "$(OS)" "Windows_NT"
	EXEEXT=.exe #on windows applications must have .exe extension
	RM=del #rm command for windows powershell
    LDFLAGS = -lfreeglut -lglu32 -lopengl32
else
	OS := $(shell uname)
	ifeq ($(OS), Darwin)
	        LDFLAGS = -framework Carbon -framework OpenGL -framework GLUT
	endif
endif

PROGRAM_NAME=flightSim

run: $(PROGRAM_NAME)
	./$(PROGRAM_NAME)$(EXEEXT)

$(PROGRAM_NAME): main.o particle.o airplane.o mathLib3D.o terrain.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -I./glm/glm.hpp

FORCE: ;

clean:
	$(RM) *.o $(PROGRAM_NAME)$(EXEEXT)