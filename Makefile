CC = clang
CFLAGS = -std=c11 -O3 -g -Wall -Wextra -Wpedantic -Wstrict-aliasing 
CFLAGS += -Wno-newline-eof -Wno-gnu-zero-variadic-macro-arguments -Wno-unused-parameter

CFLAGS += -Ilibs/glad/include -Ilibs/glfw/include

LDFLAGS = build/obj/glad.o libs/glfw/build/src/glfw3.lib
LDFLAGS += -lmsvcrt.lib -lshell32.lib -lUser32.lib -lKernel32.lib -lGdi32.lib -lAdvapi32.lib -lComctl32.lib -lOle32.lib

SRC_DIR = src
OBJ_DIR = build/obj
BIN_DIR = build/bin

SRC = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/**/*.c) $(wildcard $(SRC_DIR)/**/**/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))
BIN = bin

.PHONY: libs clean

run: all
	$(BIN)/game

all: libs game

libs:
	$(CC) -o $(OBJ_DIR)/glad.o -Ilibs/glad/include -c libs/glad/src/glad.c
	# ADD GLFW

game: $(OBJ)
	$(CC) -o $(BIN_DIR)/game $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_DIR)/*.o
	rm -f $(OBJ_DIR)/*.lib
	rm -f $(OBJ_DIR)/*.a