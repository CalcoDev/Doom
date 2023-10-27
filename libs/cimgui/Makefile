CXXC = clang++

SRC_LIB_DIR = "replace"
OBJ_LIB_DIR = "replace"

CXXCFLAGS = -I$(SRC_LIB_DIR)/glfw/include \
	-I$(SRC_LIB_DIR)/cimgui/imgui/backends \
	-I$(SRC_LIB_DIR)/cimgui/imgui \
	-I$(SRC_LIB_DIR)/cimgui \
	-I$(SRC_LIB_DIR)/cimgui/generator/output

ifeq ($(OS),Windows_NT)
	CXXCFLAGS += -DIMGUI_IMPL_API="extern \"C\" __declspec(dllexport)"
endif

SRC = cimgui.cpp \
	imgui/imgui.cpp \
	imgui/imgui_draw.cpp \
	imgui/imgui_demo.cpp \
	imgui/imgui_widgets.cpp \
	imgui/backends/imgui_impl_glfw.cpp \
	imgui/backends/imgui_impl_opengl3.cpp \
	imgui/imgui_tables.cpp

OBJ = $(patsubst %.cpp, $(OBJ_LIB_DIR)/cimgui/%.o, $(SRC))

.PHONY: dirs clean

all: dirs lib

clean:
	rm -rf $(OBJ_LIB_DIR)/cimgui

dirs:
	mkdir -p $(OBJ_LIB_DIR)/cimgui
	mkdir -p $(OBJ_LIB_DIR)/cimgui/imgui
	mkdir -p $(OBJ_LIB_DIR)/cimgui/imgui/backends

lib: $(OBJ)
	ar rcs $(OBJ_LIB_DIR)/cimgui/cimgui.lib \
		$(OBJ_LIB_DIR)/cimgui/cimgui.o \
		$(OBJ_LIB_DIR)/cimgui/imgui/imgui.o \
		$(OBJ_LIB_DIR)/cimgui/imgui/imgui_draw.o \
		$(OBJ_LIB_DIR)/cimgui/imgui/imgui_demo.o \
		$(OBJ_LIB_DIR)/cimgui/imgui/imgui_widgets.o \
		$(OBJ_LIB_DIR)/cimgui/imgui/backends/imgui_impl_glfw.o \
		$(OBJ_LIB_DIR)/cimgui/imgui/backends/imgui_impl_opengl3.o \
		$(OBJ_LIB_DIR)/cimgui/imgui/imgui_tables.o

$(OBJ_LIB_DIR)/cimgui/%.o: ./%.cpp
	$(CXXC) $(CXXCFLAGS) -c $^ -o $@