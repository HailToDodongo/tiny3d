BUILD_DIR=build
T3D_INST=$(shell realpath ../..)

include $(N64_INST)/include/n64.mk
include $(T3D_INST)/t3d.mk

N64_CFLAGS += -std=gnu2x

PROJECT_NAME=t3d_06_offscreen

src = main.c

assets_png = $(wildcard assets/*.png)
assets_gltf = $(wildcard assets/*.glb)
assets_conv = $(addprefix filesystem/,$(notdir $(assets_png:%.png=%.sprite))) \
			  $(addprefix filesystem/,$(notdir $(assets_ttf:%.ttf=%.font64))) \
			  $(addprefix filesystem/,$(notdir $(assets_gltf:%.glb=%.t3dm))) \
			  filesystem/video.m1v

all: $(PROJECT_NAME).z64

filesystem/%.sprite: assets/%.png
	@mkdir -p $(dir $@)
	@echo "    [SPRITE] $@"
	$(N64_MKSPRITE) $(MKSPRITE_FLAGS) -o filesystem "$<"

filesystem/%.t3dm: assets/%.glb
	@mkdir -p $(dir $@)
	@echo "    [T3D-MODEL] $@"
	$(T3D_GLTF_TO_3D) "$<" $@
	$(N64_BINDIR)/mkasset -c 2 -o filesystem $@

# For a proper conversion of an input video, see the "videoplayer" example in libdragon
filesystem/video.m1v: assets/video.m1v
	@mkdir -p $(dir $@)
	@echo "    [VIDEO] $@"
	cp assets/video.m1v filesystem/video.m1v


$(BUILD_DIR)/$(PROJECT_NAME).dfs: $(assets_conv)
$(BUILD_DIR)/$(PROJECT_NAME).elf: $(src:%.c=$(BUILD_DIR)/%.o)

$(PROJECT_NAME).z64: N64_ROM_TITLE="Tiny3D - Offscreen"
$(PROJECT_NAME).z64: $(BUILD_DIR)/$(PROJECT_NAME).dfs

clean:
	rm -rf $(BUILD_DIR) *.z64
	rm -rf filesystem

build_lib:
	rm -rf $(BUILD_DIR) *.z64
	make -C $(T3D_INST)
	make all

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
