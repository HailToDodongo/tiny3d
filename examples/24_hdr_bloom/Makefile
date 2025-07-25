BUILD_DIR=build
T3D_INST=$(shell realpath ../..)

include $(N64_INST)/include/n64.mk
include $(T3D_INST)/t3d.mk

 N64_CXXFLAGS += -std=gnu++20 -Os -fno-exceptions

PROJECT_NAME=t3d_24_hdr_bloom

src = $(wildcard src/*.cpp) $(wildcard src/render/*.cpp) $(wildcard src/rsp/*.cpp)
src += $(wildcard src/scene/*.cpp) $(wildcard src/camera/*.cpp) $(wildcard src/actors/*.cpp)
src += $(wildcard src/memory/*.cpp) $(wildcard src/scene/scenes/*.cpp)

assets_png = $(wildcard assets/*.png) $(wildcard assets/env/*.png) $(wildcard assets/space/*.png)
assets_png += $(wildcard assets/magic/*.png) $(wildcard assets/skyAnim/*.png)

assets_gltf = $(wildcard assets/*.glb) $(wildcard assets/space/*.glb) $(wildcard assets/skyAnim/*.glb)
assets_ttf = $(wildcard assets/*.ttf)

assets_conv = $(patsubst assets/%,filesystem/%,$(assets_png:%.png=%.sprite)) \
			  $(addprefix filesystem/,$(notdir $(assets_ttf:%.ttf=%.font64))) \
			  $(patsubst assets/%,filesystem/%,$(assets_gltf:%.glb=%.t3dm))

all: $(PROJECT_NAME).z64

filesystem/scene.t3dm: T3DM_FLAGS += --bvh --base-scale=32

filesystem/%.sprite: assets/%.png
	@mkdir -p $(dir $@)
	@echo "    [SPRITE] $@"
	$(N64_MKSPRITE) $(MKSPRITE_FLAGS) -o $(dir $@) "$<"

filesystem/%.font64: assets/%.ttf
	@mkdir -p $(dir $@)
	@echo "    [FONT] $@"
	$(N64_MKFONT) $(MKFONT_FLAGS) -s 9 -o filesystem "$<"

filesystem/%.t3dm: assets/%.glb
	@mkdir -p $(dir $@)
	@echo "    [T3D-MODEL] $@"
	$(T3D_GLTF_TO_3D) "$<" $@ $(T3DM_FLAGS)
	$(N64_BINDIR)/mkasset -c 1 -o $(dir $@) $@

$(BUILD_DIR)/$(PROJECT_NAME).dfs: $(assets_conv)
$(BUILD_DIR)/$(PROJECT_NAME).elf: $(src:%.cpp=$(BUILD_DIR)/%.o) $(BUILD_DIR)/src/rsp/rsp_fx.o

$(PROJECT_NAME).z64: N64_ROM_TITLE="Tiny3D - HDR Bloom"
$(PROJECT_NAME).z64: $(BUILD_DIR)/$(PROJECT_NAME).dfs

clean:
	rm -rf $(BUILD_DIR) *.z64
	rm -rf filesystem

build_lib:
	rm -rf $(BUILD_DIR) *.z64
	make -C $(T3D_INST)
	make all -j4

sc64:
	make
	curl 192.168.0.6:9065/off
	sleep 1
	sc64deployer --remote 192.168.0.6:9064 upload --tv ntsc *.z64
	curl 192.168.0.6:9065/on

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
