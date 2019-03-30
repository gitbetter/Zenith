SRC_DIR=_Source
H_DIR=_Headers
OBJ_DIR=_Bin

CXX=g++

RM=rm -r

ifeq ($(OS),Windows_NT)
CPPFLAGS= -std=c++14 -Wall -g $(shell dir $(H_DIR) -Attributes Directory -Name | sed s/^/-I/)
else
CPPFLAGS= -std=c++14 -Wall -g $(shell find $(H_DIR) -type d | sed s/^/-I/)
endif

LDFLAGS=-g -L./_Drivers/lib/
LDLIBS=-lglew -lglfw

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

CPP=$(call rwildcard,$(SRC_DIR),*.cpp)
OBJS=$(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CPP))

_dummy := $(shell mkdir -p $(OBJ_DIR)/{Components,Core,GameObjects,Graphics/Strategies,Utility/ZObjectFormatTools,Input,UI/Elements,UI/Decorators,Windowing,Physics/Forces,Physics/Collision,ResourceCache/ResourceFiles,ResourceCache/ResourceLoaders,EventAgent/Events,Scripting,Process})

ifeq ($(shell uname -s),Darwin)
	LDLIBS += -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo -framework CoreFoundation
endif

ifeq ($(OS),Windows_NT)
	LDLIBS += -lGL -lgdi32
endif

LDLIBS += -lbz2 -lz -lirrxml -lfreetyped -lassimp -lzip -llua
LDLIBS += -lLinearMath -lBulletCollision -lBulletDynamics

all: game

game: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

clean:
	$(RM) $(OBJ_DIR)/{Components,Core,GameObjects,Graphics,Utility,UI,Input,Physics,Windowing,ResourceCache,EventAgent,Scripting,Process,*.o} game
