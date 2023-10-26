BIN = bin
SRC_DIR = src
SRC_LIB_DIR = libs
OBJ_DIR = $(BIN)/obj
OBJ_LIB_DIR = $(BIN)/libs

CC = clang
CFLAGS = -std=c11 -O3 -g -Wall -Wextra -Wpedantic -Wstrict-aliasing 
CFLAGS += -Wno-newline-eof -Wno-gnu-zero-variadic-macro-arguments \
	-Wno-unused-parameter
CFLAGS += -I$(SRC_LIB_DIR)/glad/include \
	-I$(SRC_LIB_DIR)/glfw/include \
	-I$(SRC_LIB_DIR)/cimgui/imgui/backends \
	-I$(SRC_LIB_DIR)/cimgui/imgui \
	-I$(SRC_LIB_DIR)/cimgui \
	-I$(SRC_LIB_DIR)/cimgui/generator/output

CXXC = clang++

LDFLAGS = $(OBJ_LIB_DIR)/glad.o \
	$(OBJ_LIB_DIR)/glfw/src/glfw3.lib \
	$(OBJ_LIB_DIR)/cimgui/cimgui.lib

ifeq ($(OS),Windows_NT)
	LDFLAGS += -lmsvcrt.lib -lshell32.lib -lUser32.lib -lKernel32.lib \
		-lGdi32.lib -lAdvapi32.lib -lComctl32.lib -lOle32.lib
endif

CMFLAGS = -G "Unix Makefiles" -DCMAKE_C_COMPILER=clang

SRC  = $(shell find $(SRC_DIR) -type f -name "*.c")
OBJ  = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

CIMGUICXXFLAGS = -I$(SRC_LIB_DIR)/glfw/include \
	-I$(SRC_LIB_DIR)/cimgui/imgui/backends \
	-I$(SRC_LIB_DIR)/cimgui/imgui \
	-I$(SRC_LIB_DIR)/cimgui \
	-I$(SRC_LIB_DIR)/cimgui/generator/output

ifeq ($(OS),Windows_NT)
	CIMGUICXXFLAGS += -DIMGUI_IMPL_API="extern \"C\" __declspec(dllexport)"
endif

# Not that cache efficient are we?
.PHONY: all libs dirs clean cleanobjs cleanlibs

run: all
	$(BIN)/game

all: dirs libs game

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
	make -C $(BIN)/libs/glfw
	
# do cimgui - HMMM, ALWAYS BUILDING THIS???
	mkdir -p $(OBJ_LIB_DIR)/cimgui
	mkdir -p $(OBJ_LIB_DIR)/cimgui/imgui
	mkdir -p $(OBJ_LIB_DIR)/cimgui/imgui/backends

	$(CXXC) $(CIMGUICXXFLAGS) \
		-c $(SRC_LIB_DIR)/cimgui/cimgui.cpp \
		-o $(OBJ_LIB_DIR)/cimgui/cimgui.o
	$(CXXC) $(CIMGUICXXFLAGS) \
		-c $(SRC_LIB_DIR)/cimgui/imgui/imgui.cpp \
		-o $(OBJ_LIB_DIR)/cimgui/imgui/imgui.o
	$(CXXC) $(CIMGUICXXFLAGS) \
		-c $(SRC_LIB_DIR)/cimgui/imgui/imgui_draw.cpp \
		-o $(OBJ_LIB_DIR)/cimgui/imgui/imgui_draw.o
	$(CXXC) $(CIMGUICXXFLAGS) \
		-c $(SRC_LIB_DIR)/cimgui/imgui/imgui_demo.cpp \
		-o $(OBJ_LIB_DIR)/cimgui/imgui/imgui_demo.o
	$(CXXC) $(CIMGUICXXFLAGS) \
		-c $(SRC_LIB_DIR)/cimgui/imgui/imgui_widgets.cpp \
		-o $(OBJ_LIB_DIR)/cimgui/imgui/imgui_widgets.o
	$(CXXC) $(CIMGUICXXFLAGS) \
		-c $(SRC_LIB_DIR)/cimgui/imgui/backends/imgui_impl_glfw.cpp \
		-o $(OBJ_LIB_DIR)/cimgui/imgui/backends/imgui_impl_glfw.o
	$(CXXC) $(CIMGUICXXFLAGS) \
		-c $(SRC_LIB_DIR)/cimgui/imgui/backends/imgui_impl_opengl3.cpp \
		-o $(OBJ_LIB_DIR)/cimgui/imgui/backends/imgui_impl_opengl3.o
	$(CXXC) $(CIMGUICXXFLAGS) \
		-c $(SRC_LIB_DIR)/cimgui/imgui/imgui_tables.cpp \
		-o $(OBJ_LIB_DIR)/cimgui/imgui/imgui_tables.o

	ar rcs $(OBJ_LIB_DIR)/cimgui/cimgui.lib \
		$(OBJ_LIB_DIR)/cimgui/cimgui.o \
		$(OBJ_LIB_DIR)/cimgui/imgui/imgui.o \
		$(OBJ_LIB_DIR)/cimgui/imgui/imgui_draw.o \
		$(OBJ_LIB_DIR)/cimgui/imgui/imgui_demo.o \
		$(OBJ_LIB_DIR)/cimgui/imgui/imgui_widgets.o \
		$(OBJ_LIB_DIR)/cimgui/imgui/backends/imgui_impl_glfw.o \
		$(OBJ_LIB_DIR)/cimgui/imgui/backends/imgui_impl_opengl3.o \
		$(OBJ_LIB_DIR)/cimgui/imgui/imgui_tables.o

# NO MORE CIMGUI LOL
	
game: $(OBJ)
	$(CC) -o $(BIN)/game $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean: cleanlibs cleanobjs
	rm -rf $(BIN)/game

cleanobjs:
	rm -rf $(OBJ_DIR)

cleanlibs:
	rm -rf $(BIN)/libs