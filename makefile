COMPILE_CMD = x86_64-w64-mingw32-g++
LINK_CMD = x86_64-w64-mingw32-g++
OBJ_DIR = bin/obj
OUT_DIR = bin/out
DEBUG_CC_FLAGS = -ggdb -c -Wall -D cdwDebugMode -D cdwTestBuild
RELEASE_CC_FLAGS = -O3 -c -Wall
DEBUG_LNK_FLAGS_POST = -ggdb -static-libgcc -static-libstdc++ -static
RELEASE_LNK_FLAGS_POST = -static-libgcc -static-libstdc++ -static

debug: \
	dirs \
	$(OUT_DIR)/debug/crayon.exe \
	$(OUT_DIR)/debug/gdiapi.dll \

all: \
	debug \
	$(OUT_DIR)/release/crayon.exe \
	$(OUT_DIR)/release/gdiapi.dll \

clean:
	rm -rf bin

dirs:
	@mkdir -p $(OBJ_DIR)/debug/crayon
	@mkdir -p $(OBJ_DIR)/debug/frontend
	@mkdir -p $(OBJ_DIR)/debug/gdi
	@mkdir -p $(OBJ_DIR)/debug/graphics
	@mkdir -p $(OBJ_DIR)/release/crayon
	@mkdir -p $(OBJ_DIR)/release/frontend
	@mkdir -p $(OBJ_DIR)/release/gdi
	@mkdir -p $(OBJ_DIR)/release/graphics
	@mkdir -p $(OUT_DIR)/debug
	@mkdir -p $(OUT_DIR)/release

.PHONY: debug all clean dirs

# ----------------------------------------------------------------------
# crayon

CRAYON_SRC = \
	src/crayon/cfile.cpp \
	src/crayon/executor.cpp \
	src/crayon/log.cpp \
	src/crayon/main.cpp \
	src/crayon/test.cpp \
	src/frontend/ast.cpp \
	src/frontend/attr.cpp \
	src/frontend/dumpVisitor.cpp \
	src/frontend/lexor.cpp \
	src/frontend/parser.cpp \
	src/graphics/graphicsApi.cpp \

CRAYON_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(CRAYON_SRC)))

$(OUT_DIR)/debug/crayon.exe: $(CRAYON_DEBUG_OBJ)
	$(info $< --> $@)
	@$(LINK_CMD) -o $@ $(CRAYON_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST)

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

# ----------------------------------------------------------------------
# gdiapi

GDIAPI_SRC = \
	src/gdi/api.cpp \

GDIAPI_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(GDIAPI_SRC)))

$(OUT_DIR)/debug/gdiapi.dll: $(GDIAPI_DEBUG_OBJ)
	$(info $< --> $@)
	@$(LINK_CMD) -shared -o $@ $(GDIAPI_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST) -lgdi32

$(GDIAPI_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

GDIAPI_RELEASE_OBJ = $(subst src,$(OBJ_DIR)/release,$(patsubst %.cpp,%.o,$(GDIAPI_SRC)))

$(OUT_DIR)/release/gdiapi.dll: $(GDIAPI_RELEASE_OBJ)
	$(info $< --> $@)
	@$(LINK_CMD) -shared -o $@ $(GDIAPI_RELEASE_OBJ) $(RELEASE_LNK_FLAGS_POST)

$(GDIAPI_RELEASE_OBJ): $(OBJ_DIR)/release/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(RELEASE_CC_FLAGS) $< -o $@
