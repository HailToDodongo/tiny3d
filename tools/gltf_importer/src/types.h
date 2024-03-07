/**
* @copyright 2023 - Max Bebök
* @license MIT
*/
#pragma once

typedef unsigned long long u64;
typedef   signed long long s64;
typedef unsigned int u32;
typedef   signed int s32;
typedef unsigned short u16;
typedef   signed short s16;
typedef unsigned char u8;
typedef   signed char s8;

typedef float f32;
typedef double f64;

// Testing som assumptions:
static_assert(sizeof(u64) == 8);
static_assert(sizeof(s64) == 8);
static_assert(sizeof(u32) == 4);
static_assert(sizeof(s32) == 4);
static_assert(sizeof(u16) == 2);
static_assert(sizeof(s16) == 2);
static_assert(sizeof(u8) == 1);
static_assert(sizeof(s8) == 1);

static_assert(sizeof(f32) == 4);
static_assert(sizeof(f64) == 8);

static_assert(sizeof(void*) == 8); // 64-bit
