SRC_DIR=_Source
H_DIR=_Headers
OBJ_DIR=_Bin

CXX=g++
RM=rm -rf
CPPFLAGS= -std=c++11 -Wall -g $(shell find $(H_DIR) -type d | sed s/^/-I/)
LDFLAGS=-g
LDLIBS= -L./_Drivers/lib/ -lglew -lglfw3

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

CPP=$(call rwildcard,$(SRC_DIR),*.cpp)
OBJS=$(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CPP))

_dummy := $(shell mkdir -p $(OBJ_DIR)/{Components,Core,GameObjects,Graphics/Strategies,Utility/ZObjectFormatTools,Input/InputCommands,UI,Windowing,Physics/Forces,Physics/Collision})

ifeq ($(shell uname -s),Darwin)
	LDLIBS += -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo -framework CoreFoundation
endif

ifeq ($(OS),Windows_NT)
	LDLIBS += -lGL -lgdi32
endif

LDLIBS += -lassimp -lfreetyped
LDLIBS += -lLinearMath -lBulletCollision -lBulletDynamics

all: game

game: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

clean:
	$(RM) $(OBJ_DIR)/{Components,Core,GameObjects,Graphics,Utility,UI,Input,Physics,Windowing,*.o} game
