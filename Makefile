CXX=g++
RM=rm -rf
CPPFLAGS= -std=c++11 -Wall -g -I_Headers/Core -I_Headers/GameObjects -I_Headers/Graphics -I_Headers/Utility -I_Headers/Windowing -I_Headers/Input
LDFLAGS=-g
LDLIBS=-lglew -lglfw3
SRC_DIR=_Source
OBJ_DIR=_Bin

CPP=$(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*/*.cpp)
OBJS=$(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CPP))

ifeq ($(shell uname -s),Darwin)
	LDLIBS += -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo -framework CoreFoundation
endif

ifeq ($(OS),Windows_NT)
	LDLIBS += -lGL -lgdi32
endif

LDLIBS += -lassimp

$(shell mkdir -p $(OBJ_DIR)/{Core,GameObjects,Graphics,Utility,Windowing,Input})

all: game

game: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

clean:
	$(RM) $(OBJ_DIR)/{Core,GameObjects,Graphics,Utility,Windowing,Input,*.o} game
