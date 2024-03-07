BUILD_DIR=build
T3D_INST=$(shell realpath ../..)

include $(N64_INST)/include/n64.mk
include $(T3D_INST)/t3d.mk

N64_CFLAGS += -std=gnu2x -Os

src = main.c

all: t3d_00_quad.z64

$(BUILD_DIR)/t3d_00_quad.elf: $(src:%.c=$(BUILD_DIR)/%.o)

t3d_00_quad.z64: N64_ROM_TITLE="Tiny3D - Quad"

clean:
	rm -rf $(BUILD_DIR) *.z64

build_lib:
	rm -rf $(BUILD_DIR) *.z64
	make -C $(T3D_INST)
	make all

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
