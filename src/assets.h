#pragma once

// _DIR are defined in cmake
// ===============================================================
// textures
// ===============================================================

#define texture_dice        (string(TEXTURES_DIR) + "/dice.png").c_str()
#define texture_greentop    (string(TEXTURES_DIR) + "/greentop.png").c_str()
#define texture_redtop      (string(TEXTURES_DIR) + "/redtop.png").c_str()
#define texture_anim_00     (string(TEXTURES_DIR) + "/anim_00.png").c_str()

// ===============================================================
// shaders
// ===============================================================

#define shader_default_vs   (string(SHADER_DIR) + "/default.vs").c_str()
#define shader_default_fs   (string(SHADER_DIR) + "/default.fs").c_str()

#define shader_jolt_vs      (string(SHADER_DIR) + "/default_jolt.vs").c_str()
#define shader_jolt_fs      (string(SHADER_DIR) + "/default_jolt.fs").c_str()

#define shader_bb_vs        (string(SHADER_DIR) + "/default_bb.vs").c_str()
#define shader_bb_fs        (string(SHADER_DIR) + "/default_bb.fs").c_str()

#define shader_nBindless_fs (string(SHADER_DIR) + "/nBindless.fs").c_str()

#define UINB_fs (string(SHADER_DIR) + "/user_interface.fs").c_str()
#define UINB_vs (string(SHADER_DIR) + "/user_interface.vs").c_str()

// ===============================================================
// shaders v2
// ===============================================================
// doesnt use bindless texture as of oct 29 subject to change

#define v2_default_vs        (string(SHADER_DIR) + "/2_default.vs").c_str()
#define v2_default_fs        (string(SHADER_DIR) + "/2_default.fs").c_str()

// ===============================================================
// models
// ===============================================================

#define model_cube          (string(MODELS_DIR) + "/cube.gltf").c_str()
#define model_cone          (string(MODELS_DIR) + "/cone.gltf").c_str()