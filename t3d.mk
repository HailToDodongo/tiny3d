N64_LDFLAGS := $(T3D_INST)/build/t3d.a $(N64_LDFLAGS)
N64_C_AND_CXX_FLAGS += -I$(T3D_INST)/src

T3D_GLTF_TO_3D = $(T3D_INST)/tools/gltf_importer/gltf_to_t3d