PROJECT_NAME := eulerian-fluid-simulator
BUILD_DIR := build
BIN_DIR := bin
RELEASE_TARGET := $(BIN_DIR)/$(PROJECT_NAME)
DEBUG_TARGET := $(BIN_DIR)/$(PROJECT_NAME)-debug

CXX ?= g++
USE_OPENMP ?= 1
USE_JACOBI ?= 0
STD := -std=c++17
WARNINGS := -Wall -Wextra -Wpedantic
OPENMP_FLAGS := $(if $(filter 1,$(USE_OPENMP)),-fopenmp,)
SOLVER_FLAGS := $(if $(filter 1,$(USE_JACOBI)),-DFLUID_USE_JACOBI,)
COMMON_FLAGS := $(STD) $(WARNINGS) $(OPENMP_FLAGS) $(SOLVER_FLAGS)

RELEASE_FLAGS := -O3 -DNDEBUG
DEBUG_FLAGS := -O0 -g3

SRC := main.cpp
OBJ_RELEASE := $(addprefix $(BUILD_DIR)/release/,$(SRC:.cpp=.o))
OBJ_DEBUG := $(addprefix $(BUILD_DIR)/debug/,$(SRC:.cpp=.o))

PKG_CONFIG := $(shell command -v pkg-config 2>/dev/null)

ifeq ($(PKG_CONFIG),)
  SFML_CFLAGS :=
  SFML_LIBS := -lsfml-graphics -lsfml-window -lsfml-system
else
	SFML_CFLAGS_RAW := $(shell pkg-config --cflags sfml-graphics 2>/dev/null)
	SFML_LIBS_RAW := $(shell pkg-config --libs sfml-graphics 2>/dev/null)

	ifneq ($(strip $(SFML_LIBS_RAW)),)
		SFML_CFLAGS := $(SFML_CFLAGS_RAW)
		SFML_LIBS := $(SFML_LIBS_RAW)
	else
		SFML_CFLAGS :=
		SFML_LIBS := -lsfml-graphics -lsfml-window -lsfml-system
	endif
endif

.PHONY: all release debug run test clean info

all: release

release: CXXFLAGS := $(COMMON_FLAGS) $(RELEASE_FLAGS) $(SFML_CFLAGS)
release: LDFLAGS := $(SFML_LIBS) $(OPENMP_FLAGS)
release: $(RELEASE_TARGET)

debug: CXXFLAGS := $(COMMON_FLAGS) $(DEBUG_FLAGS) $(SFML_CFLAGS)
debug: LDFLAGS := $(SFML_LIBS) $(OPENMP_FLAGS)
debug: $(DEBUG_TARGET)

$(BIN_DIR) $(BUILD_DIR)/release $(BUILD_DIR)/debug:
	@mkdir -p $@

$(BUILD_DIR)/release/%.o: %.cpp | $(BUILD_DIR)/release
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/debug/%.o: %.cpp | $(BUILD_DIR)/debug
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(RELEASE_TARGET): $(OBJ_RELEASE) | $(BIN_DIR)
	$(CXX) $^ -o $@ $(LDFLAGS)

$(DEBUG_TARGET): $(OBJ_DEBUG) | $(BIN_DIR)
	$(CXX) $^ -o $@ $(LDFLAGS)

run: release
	./$(RELEASE_TARGET)

# No automated unit tests yet: this target validates compilation in debug mode.
test: debug
	@echo "[test] Build validation succeeded: $(DEBUG_TARGET)"

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

info:
	@echo "Compiler: $(CXX)"
	@echo "OpenMP enabled: $(USE_OPENMP)"
	@echo "Jacobi fallback: $(USE_JACOBI)"
	@echo "SFML CFLAGS: $(SFML_CFLAGS)"
	@echo "SFML LIBS: $(SFML_LIBS)"
	@echo "Release target: $(RELEASE_TARGET)"
	@echo "Debug target: $(DEBUG_TARGET)"
