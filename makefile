COMPILE_CMD = x86_64-w64-mingw32-g++
LINK_CMD = x86_64-w64-mingw32-g++
OBJ_DIR = bin/obj
OUT_DIR = bin/out
DEBUG_CC_FLAGS = -ggdb -c -Wall -D cdwDebugMode
RELEASE_CC_FLAGS = -O3 -c -Wall
DEBUG_LNK_FLAGS_POST = -ggdb -static-libgcc -static-libstdc++ -static
RELEASE_LNK_FLAGS_POST = -static-libgcc -static-libstdc++ -static

debug: \
	dirs \
	$(OUT_DIR)/debug/crayon.exe \

all: \
	debug \
	$(OUT_DIR)/release/crayon.exe \

clean:
	rm -rf bin

dirs:
	@mkdir -p $(OBJ_DIR)/debug/crayon
	@mkdir -p $(OBJ_DIR)/release/crayon
	@mkdir -p $(OUT_DIR)/debug
	@mkdir -p $(OUT_DIR)/release

.PHONY: debug all clean dirs

# ----------------------------------------------------------------------
# crayon

CRAYON_SRC = \
	src/crayon/main.cpp \

CRAYON_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(CRAYON_SRC)))

$(OUT_DIR)/debug/crayon.exe: $(CRAYON_DEBUG_OBJ)
	$(info $< --> $@)
	@$(LINK_CMD) -o $@ $(CRAYON_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST) -lgdi32

$(CRAYON_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

CRAYON_RELEASE_OBJ = $(subst src,$(OBJ_DIR)/release,$(patsubst %.cpp,%.o,$(CRAYON_SRC)))

$(OUT_DIR)/release/crayon.exe: $(CRAYON_RELEASE_OBJ)
	$(info $< --> $@)
	@$(LINK_CMD) -o $@ $(CRAYON_RELEASE_OBJ) $(RELEASE_LNK_FLAGS_POST)

$(CRAYON_RELEASE_OBJ): $(OBJ_DIR)/release/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(RELEASE_CC_FLAGS) $< -o $@
