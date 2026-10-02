###############################################################################
# Declare some Makefile variables
###############################################################################
CC = g++
LANG_STD = -std=c++17
COMPILER_FLAGS = -Wall -Wfatal-errors -g
INCLUDE_PATH = -I/opt/homebrew/include -I/opt/homebrew/opt/lua@5.4/include/lua -I"./libs/"

# Dynamically find all .cpp files recursively so you don't have to update this list
SRC_FILES = $(shell find src -name "*.cpp")

LINKER_FLAGS = -L/opt/homebrew/opt/lua@5.4/lib -L/opt/homebrew/lib -lSDL2 -lSDL2_image -lSDL2_ttf -llua
OBJ_NAME = gameengine

###############################################################################
# Declare some Makefile rules
###############################################################################
build:
	find src -type f \( -name "*.cpp" -o -name "*.h" \) -exec clang-format -i {} +
	$(CC) $(COMPILER_FLAGS) $(LANG_STD) $(INCLUDE_PATH) $(SRC_FILES) $(LINKER_FLAGS) -o $(OBJ_NAME)

run:
	./$(OBJ_NAME)

clean:
	rm -f $(OBJ_NAME)
