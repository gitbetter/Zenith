CXX=g++
RM=rm -f
CPPFLAGS=-Wall -g -IHeaders
LDFLAGS=-g
LDLIBS=-lglew -lglfw3 
SRC_DIR=Source
OBJ_DIR=Bin

CPP=$(wildcard $(SRC_DIR)/*.cpp)
OBJS=$(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CPP))

ifeq ($(shell uname -s),Darwin)
	LDLIBS += -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo -framework CoreFoundation
endif


$(shell mkdir $(OBJ_DIR))

all: game

game: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

clean:
	$(RM) $(OBJS) game
