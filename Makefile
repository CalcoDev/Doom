BIN = bin
SRC_DIR = esrc
OBJ_DIR = $(BIN)/obj

CC = clang
CFLAGS = -std=c11 -O3 -g -Wall -Wextra -Wpedantic -Wstrict-aliasing 
CFLAGS += -Wno-newline-eof -Wno-gnu-zero-variadic-macro-arguments -Wno-unused-parameter
CFLAGS += -Ilibs/glad/include -Ilibs/glfw/include

LDFLAGS = $(OBJ_DIR)/glad.o $(BIN)/libs/glfw/src/glfw3.lib
LDFLAGS += -lmsvcrt.lib -lshell32.lib -lUser32.lib -lKernel32.lib -lGdi32.lib -lAdvapi32.lib -lComctl32.lib -lOle32.lib

SRC  = $(shell find $(SRC_DIR) -type f -name "*.c")
OBJ  = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

.PHONY: libs clean

run: all
	$(BIN)/game

all: dirs libs game

dirs:
	mkdir -p ./$(BIN)
	mkdir -p ./$(BIN)/libs
	mkdir -p ./$(OBJ_DIR)

libs:
	$(CC) -o $(OBJ_DIR)/glad.o -Ilibs/glad/include -c libs/glad/src/glad.c
	
	cmake -S libs/glfw -B $(BIN)/libs/glfw -G "Unix Makefiles" -DCMAKE_C_COMPILER=clang -DGLFW_BUILD_DOCS=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_INSTALL=OFF \
	&& make -C $(BIN)/libs/glfw
	
	# ADD CIMGUI
	
game: $(OBJ)
	echo "Objects: $^"
	$(CC) -o $(BIN)/game $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BIN)