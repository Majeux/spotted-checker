INC_DIR			= inc
SRC_DIR			= src
MAIN_DIR		= main
SOURCES			= $(shell find $(SRC_DIR)/ -name '*.cc')
# MAIN_SRC		= main/main.cc
MAIN_SRC		= main/test-kripke.cc
OBJECTS			= $(SOURCES:.cc=.o)
MAIN_OBJ		= $(MAIN_SRC:.cc=.o)
MAIN_TARGET		= spot-check

CC				= g++
CFLAGS			= -std=c++14 -O3 -Wall -Wextra
CPPFLAGS 		= $(addprefix -I, $(INC_DIR)) -pedantic
SPOTFLAGS 		= -lspot

.PHONY: all clean

all: $(MAIN_TARGET)

%.o: %.cc
	$(CC) $(CFLAGS) $(CPPFLAGS) $(SPOTFLAGS) -c $< -o $@

$(MAIN_TARGET): $(OBJECTS) $(MAIN_OBJ)
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ $(SPOTFLAGS) -o $@

clean:
	rm -f $(OBJECTS) $(MAIN_OBJ) $(DEPS) $(MAIN_DEPS) $(MAIN_TARGET)
