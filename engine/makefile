CXX      = g++
EMCC     = emcc
INCLUDES = -Iinclude
SRC_DIR  = src
OBJ_DIR  = obj

CXXFLAGS   = -std=c++17 -Wall -Wextra -Werror -Ofast
WASM_CFLAGS  = -std=c++17 -O2 -DWASM_BUILD
WASM_LDFLAGS = -std=c++17 -O2 \
               -sEXPORTED_FUNCTIONS=_getBestMove,_malloc,_free \
               -sEXPORTED_RUNTIME_METHODS=ccall,cwrap,UTF8ToString \
               -sMODULARIZE=1 \
               -sEXPORT_NAME=ChessEngine \
               -sASSERTIONS=1 \
               -sSTACK_SIZE=1048576 \
               -sINITIAL_MEMORY=33554432 \
               -sALLOW_MEMORY_GROWTH=1

ALL_SRC     = $(wildcard $(SRC_DIR)/*.cpp)
NATIVE_SRC  = $(filter-out $(SRC_DIR)/wasm_api.cpp, $(ALL_SRC)) main.cpp
NATIVE_OBJ  = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, \
              $(filter $(SRC_DIR)/%.cpp, $(NATIVE_SRC))) \
              $(OBJ_DIR)/main.o

WASM_SRC    = $(ALL_SRC)
WASM_OBJ    = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/wasm/%.o, $(WASM_SRC))

NATIVE_TARGET = main
WASM_TARGET   = ../website/public/engine.js

.PHONY: all wasm clean

all: $(NATIVE_TARGET)

# Native build
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/main.o: main.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(NATIVE_TARGET): $(NATIVE_OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^

# WASM build
$(OBJ_DIR)/wasm/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)/wasm
	$(EMCC) $(WASM_CFLAGS) $(INCLUDES) -c $< -o $@

wasm: $(WASM_OBJ)
	@mkdir -p $(dir $(WASM_TARGET))
	$(EMCC) $(WASM_LDFLAGS) $(INCLUDES) -o $(WASM_TARGET) $^

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(NATIVE_TARGET) $(WASM_TARGET) \
	       $(patsubst %.js, %.wasm, $(WASM_TARGET))
