COMPILE_CMD = x86_64-w64-mingw32-g++
LINK_CMD = x86_64-w64-mingw32-g++
OBJ_DIR = bin/obj
OUT_DIR = bin/out
DEBUG_CC_FLAGS = -ggdb -c -Wall -D cdwDebugMode -D cdwTestBuild
RELEASE_CC_FLAGS = -O3 -c -Wall
DEBUG_LNK_FLAGS_POST = -ggdb -static-libgcc -static-libstdc++ -static
RELEASE_LNK_FLAGS_POST = -static-libgcc -static-libstdc++ -static

test: \
	debug \
	systemtest

debug: \
	dirs \
	$(OUT_DIR)/debug/crayon.exe \
	$(OUT_DIR)/debug/gdiapi.dll \
	$(OUT_DIR)/debug/gdiplusapi.dll \

all: \
	debug \
	$(OUT_DIR)/release/crayon.exe \
	$(OUT_DIR)/release/gdiapi.dll \
	$(OUT_DIR)/release/gdiplusapi.dll \

clean:
	rm -rf bin

dirs:
	@mkdir -p $(OBJ_DIR)/debug/cmn
	@mkdir -p $(OBJ_DIR)/debug/crayon
	@mkdir -p $(OBJ_DIR)/debug/frontend
	@mkdir -p $(OBJ_DIR)/debug/gdi
	@mkdir -p $(OBJ_DIR)/debug/gdiplus
	@mkdir -p $(OBJ_DIR)/debug/graphics
	@mkdir -p $(OBJ_DIR)/debug/graphics/algorithm
	@mkdir -p $(OBJ_DIR)/release/cmn
	@mkdir -p $(OBJ_DIR)/release/crayon
	@mkdir -p $(OBJ_DIR)/release/frontend
	@mkdir -p $(OBJ_DIR)/release/gdi
	@mkdir -p $(OBJ_DIR)/release/gdiplus
	@mkdir -p $(OBJ_DIR)/release/graphics
	@mkdir -p $(OBJ_DIR)/release/graphics/algorithm
	@mkdir -p $(OUT_DIR)/debug
	@mkdir -p $(OUT_DIR)/release

.PHONY: debug all clean dirs systemtest test

# ----------------------------------------------------------------------
# cmn

CMN_SRC = \
	src/crayon/cfile.cpp \
	src/crayon/path.cpp \
	src/graphics/cmn.cpp \

CMN_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(CMN_SRC)))

$(OUT_DIR)/debug/cmn.lib: $(CMN_DEBUG_OBJ)
	$(info $< --> $@)
	@ar crs $@ $^

$(CMN_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

CMN_RELEASE_OBJ = $(subst src,$(OBJ_DIR)/release,$(patsubst %.cpp,%.o,$(CMN_SRC)))

$(OUT_DIR)/release/cmn.lib: $(CMN_RELEASE_OBJ)
	$(info $< --> $@)
	@ar crs $@ $^

$(CMN_RELEASE_OBJ): $(OBJ_DIR)/release/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(RELEASE_CC_FLAGS) $< -o $@

# ----------------------------------------------------------------------
# crayon

CRAYON_SRC = \
	src/crayon/cmnTest.cpp \
	src/crayon/executor.cpp \
	src/crayon/fileFinder.cpp \
	src/crayon/log.cpp \
	src/crayon/main.cpp \
	src/crayon/stringFileParser.cpp \
	src/crayon/symbolTable.cpp \
	src/crayon/test.cpp \
	src/frontend/ast.cpp \
	src/frontend/attr.cpp \
	src/frontend/crawler.cpp \
	src/frontend/dumpVisitor.cpp \
	src/frontend/eval.cpp \
	src/frontend/lexor.cpp \
	src/frontend/parser.cpp \
	src/graphics/algorithm/find.cpp \
	src/graphics/algorithm/frame.cpp \
	src/graphics/algorithm/analysis.cpp \
	src/graphics/algorithm/outline.cpp \
	src/graphics/algorithm/tag.cpp \
	src/graphics/algorithm/transform.cpp \
	src/graphics/algorithm/whiskers.cpp \
	src/graphics/graphicsApi.cpp \
	src/graphics/snippet.cpp \

CRAYON_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(CRAYON_SRC)))

$(OUT_DIR)/debug/crayon.exe: $(CRAYON_DEBUG_OBJ) $(OUT_DIR)/debug/cmn.lib
	$(info $< --> $@)
	@$(LINK_CMD) -o $@ $(CRAYON_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST) -Lbin/out/debug -lcmn

$(CRAYON_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

CRAYON_RELEASE_OBJ = $(subst src,$(OBJ_DIR)/release,$(patsubst %.cpp,%.o,$(CRAYON_SRC)))

$(OUT_DIR)/release/crayon.exe: $(CRAYON_RELEASE_OBJ) $(OUT_DIR)/release/cmn.lib
	$(info $< --> $@)
	@$(LINK_CMD) -o $@ $(CRAYON_RELEASE_OBJ) $(RELEASE_LNK_FLAGS_POST) -Lbin/out/release -lcmn

$(CRAYON_RELEASE_OBJ): $(OBJ_DIR)/release/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(RELEASE_CC_FLAGS) $< -o $@

# ----------------------------------------------------------------------
# gdiapi

GDIAPI_SRC = \
	src/gdi/diag.cpp \
	src/gdi/loadsave.cpp \
	src/gdi/main.cpp \

GDIAPI_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(GDIAPI_SRC)))

$(OUT_DIR)/debug/gdiapi.dll: $(GDIAPI_DEBUG_OBJ) $(OUT_DIR)/debug/cmn.lib
	$(info $< --> $@)
	@$(LINK_CMD) -shared -o $@ $(GDIAPI_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST) -Lbin/out/debug -lgdi32 -lcmn

$(GDIAPI_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

GDIAPI_RELEASE_OBJ = $(subst src,$(OBJ_DIR)/release,$(patsubst %.cpp,%.o,$(GDIAPI_SRC)))

$(OUT_DIR)/release/gdiapi.dll: $(GDIAPI_RELEASE_OBJ) $(OUT_DIR)/release/cmn.lib
	$(info $< --> $@)
	@$(LINK_CMD) -shared -o $@ $(GDIAPI_RELEASE_OBJ) $(RELEASE_LNK_FLAGS_POST) -Lbin/out/release -lgdi32 -luser32 -lcmn

$(GDIAPI_RELEASE_OBJ): $(OBJ_DIR)/release/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(RELEASE_CC_FLAGS) $< -o $@

# ----------------------------------------------------------------------
# gdiplusapi

GDIPLUSAPI_SRC = \
	src/gdiplus/main.cpp \

GDIPLUSAPI_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(GDIPLUSAPI_SRC)))

$(OUT_DIR)/debug/gdiplusapi.dll: $(GDIPLUSAPI_DEBUG_OBJ) $(OUT_DIR)/debug/cmn.lib
	$(info $< --> $@)
	@$(LINK_CMD) -shared -o $@ $(GDIPLUSAPI_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST) -Lbin/out/debug -lgdiplus -lcmn

$(GDIPLUSAPI_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

GDIPLUSAPI_RELEASE_OBJ = $(subst src,$(OBJ_DIR)/release,$(patsubst %.cpp,%.o,$(GDIPLUSAPI_SRC)))

$(OUT_DIR)/release/gdiplusapi.dll: $(GDIPLUSAPI_RELEASE_OBJ) $(OUT_DIR)/release/cmn.lib
	$(info $< --> $@)
	@$(LINK_CMD) -shared -o $@ $(GDIPLUSAPI_RELEASE_OBJ) $(RELEASE_LNK_FLAGS_POST) -Lbin/out/release -lgdiplus -luser32 -lcmn

$(GDIPLUSAPI_RELEASE_OBJ): $(OBJ_DIR)/release/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(RELEASE_CC_FLAGS) $< -o $@

# ----------------------------------------------------------------------
systemtest:
	@cmd /c systemtest.bat
