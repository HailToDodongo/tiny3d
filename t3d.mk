T3D_DIR := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))

N64_LDFLAGS := $(T3D_DIR)/build/libt3d.a $(N64_LDFLAGS)
N64_C_AND_CXX_FLAGS += -I$(T3D_DIR)/src

T3D_GLTF_TO_3D := $(T3D_DIR)/tools/gltf_importer/gltf_to_t3d
