CXX=g++
RM=rm -f
CPPFLAGS=-Wall -g -IHeaders/Core -IHeaders/GameObjects -IHeaders/Graphics -IHeaders/Utility -IHeaders/Windowing
LDFLAGS=-g
LDLIBS=-lglew -lglfw3
SRC_DIR=Source
OBJ_DIR=Bin

CPP=$(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*/*.cpp)
OBJS=$(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CPP))

ifeq ($(shell uname -s),Darwin)
	LDLIBS += -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo -framework CoreFoundation
endif

ifeq ($(OS),Windows_NT)
	LDLIBS += -lGL -lgdi32
endif

$(shell mkdir -p $(OBJ_DIR)/{Core,GameObjects,Graphics,Utility,Windowing})

all: game

game: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

# TODO: Remove subdirectories as well for a clean slate
clean:
	$(RM) $(OBJS) game
	#$(RM) $(OBJ_DIR)/{Core,GameObjects,Graphics,Utility,Windowing} game
