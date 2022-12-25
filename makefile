#
#  Run 'make' to compile and link
#  Or 'make debug' to compile with debug symbols
#
#SOURCES
OBJS= src/*.cpp src/vendor/*.cpp src/vendor/imgui/*.cpp src/tests/*.cpp

#COMPILER
CC= g++ # Or clang++
#COMPILER FLAGS
CXXF=-Wall -std=c++11 -std=c++14 -std=c++17 -fexceptions

LDF= $(pkg-config --static --libs glfw3)

all: executable

# Add -g flag to generate debug symbols on debug build
debug: CXXF += -DDEBUG -g
debug: executable

$(PCH_OUT): $(PCH_SRC) $(PCH_HEADERS)
	$(CC) $(CXXF) -o $@ $<

UNAME := $(shell uname -s)
# For Mac
ifeq ($(UNAME),Darwin)
LDF+= -lGL -lGLU -lglut -lGLEW -lm ld# -lglu32
else ifeq ($(UNAME),Linux)
LDF+= -lGL -lGLU -lglut -lGLEW -lm -lglu32
else
# For Windows using MSYS2/MING or Cygwin
LDF+= -lmingw32 -mwindows -lglfw3 -lgdi32 -lglew32  -lopengl32 -lm
endif

INCLUDE_PATHS = -I/usr/local/include -I/opt/X11/include -Isrc/vendor
LIBRARY_PATHS = -L/usr/local/lib -L/opt/X11/lib -Lsrc/lib
OBJ_NAME= main

executable : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(CXXF) $(LDF) -o $(OBJ_NAME)
