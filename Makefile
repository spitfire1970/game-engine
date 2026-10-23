###############################################################################
# Declare some Makefile variables
###############################################################################
CC = g++
LANG_STD = -std=c++17
COMPILER_FLAGS = -Wall -Wfatal-errors -g -O0

UNAME_S := $(shell uname -s)

IMGUI_SRC = $(shell find libs/imgui -name "*.cpp")

ifeq ($(UNAME_S),Darwin)
    INCLUDE_PATH = -I/opt/homebrew/include -I/opt/homebrew/opt/lua@5.4/include/lua -I"./libs/" -I/opt/homebrew/include/SDL2
    LINKER_FLAGS = -L/opt/homebrew/opt/lua@5.4/lib -L/opt/homebrew/lib -lSDL2 -lSDL2_image -lSDL2_ttf -llua
else
    INCLUDE_PATH = -I/usr/include/lua5.4 -I"./libs/"
    LINKER_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -llua5.4
endif


# Dynamically find all .cpp files recursively so you don't have to update this list
SRC_FILES = $(shell find src -name "*.cpp")

OBJ_NAME = gameengine

###############################################################################
# Declare some Makefile rules
###############################################################################
build:
	find src -type f \( -name "*.cpp" -o -name "*.h" \) -exec clang-format -i {} +
	$(CC) $(COMPILER_FLAGS) $(LANG_STD) $(INCLUDE_PATH) $(SRC_FILES) $(IMGUI_SRC) $(LINKER_FLAGS) -o $(OBJ_NAME)

run:
	./$(OBJ_NAME)

clean:
	rm -f $(OBJ_NAME)
