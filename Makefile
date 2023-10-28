BIN = bin
SRC_DIR = src
SRC_LIB_DIR = libs
OBJ_DIR = $(BIN)/obj
OBJ_LIB_DIR = $(BIN)/libs
ASSETS_DIR = assets

CC = clang --target=x86_64-pc-windows-msvc -g -O0
CFLAGS = -Wall -Wextra -Wpedantic -Wstrict-aliasing 
CFLAGS += -Wno-newline-eof -Wno-gnu-zero-variadic-macro-arguments \
	-Wno-unused-parameter
CFLAGS += -I$(SRC_LIB_DIR)/glad/include \
	-I$(SRC_LIB_DIR)/glfw/include \
	-I$(SRC_LIB_DIR)/cimgui/imgui/backends \
	-I$(SRC_LIB_DIR)/cimgui/imgui \
	-I$(SRC_LIB_DIR)/cimgui \
	-I$(SRC_LIB_DIR)/cimgui/generator/output \
	-I$(SRC_LIB_DIR)/soloud/include

LDFLAGS = $(OBJ_LIB_DIR)/glad.o \
	$(OBJ_LIB_DIR)/glfw/src/glfw3.lib \
	$(OBJ_LIB_DIR)/cimgui/cimgui.lib \
	$(OBJ_LIB_DIR)/soloud/soloud.lib

ifeq ($(OS),Windows_NT)
	LDFLAGS += -lmsvcrt.lib -lshell32.lib -lUser32.lib -lKernel32.lib \
		-lGdi32.lib -lAdvapi32.lib -lComctl32.lib -lOle32.lib -lShLwApi.lib \
		-lUserEnv.lib -lWindowsApp.lib
endif

CMFLAGS = -G "Unix Makefiles" -DCMAKE_C_COMPILER=clang

SRC  = $(shell find $(SRC_DIR) -type f -name "*.c")
OBJ  = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

# Not that cache efficient are we?
.PHONY: all dirs libs game assets clean cleanobjs cleanlibs

run: all
	$(BIN)/game

all: dirs libs game assets

dirs:
	mkdir -p ./$(BIN)
	mkdir -p ./$(OBJ_DIR)
	mkdir -p ./$(OBJ_LIB_DIR)

libs:
# do glad - HMMM, ALWAYS BUILDING THIS???
	$(CC) -o $(OBJ_LIB_DIR)/glad.o -Ilibs/glad/include -c \
		$(SRC_LIB_DIR)/glad/src/glad.c
	
# go glfw
	cmake $(CMFLAGS) -S $(SRC_LIB_DIR)/glfw -B $(OBJ_LIB_DIR)/glfw \
		-DGLFW_BUILD_DOCS=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_EXAMPLES=OFF \
		-DGLFW_INSTALL=OFF
	make -C $(OBJ_LIB_DIR)/glfw
	
# do cimgui
	make -C $(SRC_LIB_DIR)/cimgui all -e SRC_LIB_DIR=../../$(SRC_LIB_DIR) -e OBJ_LIB_DIR=../../$(OBJ_LIB_DIR)

# do soloud
	cmake $(CMFLAGS) -DCMAKE_CXX_COMPILER=clang++ \
		-S $(SRC_LIB_DIR)/soloud/contrib -B $(OBJ_LIB_DIR)/soloud \
		-DSOLOUD_BUILD_DEMOS=ON -DSOLOUD_GENERATE_GLUE=OFF -DSOLOUD_STATIC=ON \
		-DSOLOUD_C_API=ON -DSOLOUD_BACKEND_NULL=OFF -DSOLOUD_BACKEND_SDL2=OFF \
		-DSOLOUD_BACKEND_ALSA=OFF -DSOLOUD_BACKEND_COREAUDIO=OFF \
		-DSOLOUD_BACKEND_OPENSLES=OFF -DSOLOUD_BACKEND_XAUDIO2=OFF \
		-DSOLOUD_BACKEND_WINMM=OFF -DSOLOUD_BACKEND_WASAPI=ON
	make -C $(OBJ_LIB_DIR)/soloud
	
game: $(OBJ)
	$(CC) -o $(BIN)/game $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

assets:
	mkdir -p $(BIN)/assets;

	rm -rf $(BIN)/assets;
	cp -r $(ASSETS_DIR)/. $(BIN)/assets;

clean: cleanlibs cleanobjs
	rm -rf $(BIN)/*

cleanobjs:
	rm -rf $(OBJ_DIR)

cleanlibs:
	rm -rf $(BIN)/libs