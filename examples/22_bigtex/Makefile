BUILD_DIR=build
T3D_INST=$(shell realpath ../..)

include $(N64_INST)/include/n64.mk
include $(T3D_INST)/t3d.mk

N64_CXXFLAGS += -std=gnu++20 -ftrivial-auto-var-init=uninitialized -fno-exceptions -Os -Isrc

PROJECT_NAME=t3d_22_bigtex

src =  $(wildcard src/*.cpp) $(wildcard src/**/*.cpp) $(wildcard src/**/**/*.cpp)

assets_png = $(wildcard assets/*.rgba16.png) $(wildcard assets/*.ia4.png) $(wildcard assets/*.ci4.png)
assets_glb = $(wildcard assets/*.glb) $(wildcard assets/obj/*.glb) $(wildcard assets/map/**/*.glb)
assets_ttf = $(wildcard assets/*.ttf)
assets_mp3 = $(wildcard assets/bgm/*.mp3)
assets_wav = $(wildcard assets/sfx/*.wav)
assets_bci = $(wildcard assets/*.bci.png) $(wildcard assets/skybox/*/*.bci.png)

assets_conv = $(patsubst assets/%,filesystem/%,$(assets_png:%.png=%.sprite)) \
			  $(patsubst assets/%,filesystem/%,$(assets_glb:%.glb=%.t3dm)) \
			  $(patsubst assets/%,filesystem/%,$(assets_ttf:%.ttf=%.font64)) \
			  $(patsubst assets/%,filesystem/%,$(assets_mp3:%.mp3=%.wav64)) \
			  $(patsubst assets/%,filesystem/%,$(assets_wav:%.wav=%.wav64)) \
			  $(patsubst assets/%,filesystem/%,$(assets_bci:%.png=%))

# $(info $$assets_conv is [${assets_conv}])

all: $(PROJECT_NAME).z64

filesystem/%.sprite: assets/%.png
	@mkdir -p $(dir $@)
	@echo "    [SPRITE] $@"
	$(N64_MKSPRITE) $(MKSPRITE_FLAGS) -o $(dir $@) "$<"

filesystem/%.t3dm: assets/%.glb
	@mkdir -p $(dir $@)
	@echo "    [T3D-MODEL]"
	$(T3D_GLTF_TO_3D) "$<" $@ $(T3DM_FLAGS)
	$(N64_BINDIR)/mkasset -c 2 -o $(dir $@) $@

filesystem/%.font64: assets/%.ttf
	@mkdir -p $(dir $@)
	@echo "    [FONT] $@"
	@$(N64_MKFONT) $(MKFONT_FLAGS) -o $(dir $@) "$<"

AUDIOCONV_FLAGS = --wav-resample 22050 --wav-mono

filesystem/bgm/%.wav64: assets/bgm/%.mp3
	@mkdir -p $(dir $@)
	@echo "    [SFX] $@"
	$(N64_AUDIOCONV) $(AUDIOCONV_FLAGS) -o $(dir $@) "$<"

filesystem/sfx/%.wav64: assets/sfx/%.wav
	@mkdir -p $(dir $@)
	@echo "    [SFX] $@"
	$(N64_AUDIOCONV) $(AUDIOCONV_FLAGS) --wav-compress 0 -o $(dir $@) "$<"
	# $(N64_BINDIR)/mkasset -c 3 -o $(dir $@) $@

filesystem/sfx/%.wav64: assets/sfx/%.wav
	@mkdir -p $(dir $@)
	@echo "    [SFX] $@"
	$(N64_AUDIOCONV) $(AUDIOCONV_FLAGS) --wav-compress 0 -o $(dir $@) "$<"
	# $(N64_BINDIR)/mkasset -c 3 -o $(dir $@) $@

filesystem/%.bci: assets/%.bci.png
	@mkdir -p $(dir $@)
	@echo "    [HD-IMG] $@ $<"
	./tools/imgconv/imgconv "$<" $@
	$(N64_BINDIR)/mkasset -c 2 -o $(dir $@) $@

build/%.dfs:
	@mkdir -p $(dir $@)
	@echo "    [DFS*] $@ $(<D)"
	$(N64_MKDFS) $@ filesystem >/dev/null

tools/imgconv/imgconv:
	@echo "    [BUILD] imgconv"
	@make -C tools/imgconv

$(assets_conv): tools/imgconv/imgconv

$(BUILD_DIR)/$(PROJECT_NAME).dfs: tools/imgconv/imgconv $(assets_conv)
$(BUILD_DIR)/$(PROJECT_NAME).elf: $(src:%.cpp=$(BUILD_DIR)/%.o) $(BUILD_DIR)/src/tex.o $(BUILD_DIR)/src/rsp/rsp_fx.o

$(PROJECT_NAME).z64: N64_ROM_TITLE="Tiny3D - Big-Tex"
$(PROJECT_NAME).z64: $(BUILD_DIR)/$(PROJECT_NAME).dfs

clean:
	rm -rf $(BUILD_DIR) $(PROJECT_NAME).z64
	rm -rf filesystem
	@make -C tools/imgconv clean

build_lib:
	rm -rf $(BUILD_DIR) $(PROJECT_NAME).z64
	make -C $(T3D_INST)
	make all

run:
	make
	$(N64_EMU) $(PROJECT_NAME).z64

deploy:
	make -j12
	curl 192.168.0.6:9065/off
	sleep 1
	sc64deployer --remote 192.168.0.6:9064 upload --tv=ntsc *.z64
	curl 192.168.0.6:9065/on
	sc64deployer --remote 192.168.0.6:9064 debug

-include $(wildcard $(BUILD_DIR)/src/*.d)

.PHONY: all clean
