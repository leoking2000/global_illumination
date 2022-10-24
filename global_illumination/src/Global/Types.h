#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#define PI glm::pi<f32>()

// int types

// 8 bit interger
typedef char i8;
typedef unsigned char u8;

// 16 bit interger
typedef short i16;
typedef unsigned short u16;

// 32 bit interger
typedef int i32;
typedef unsigned int u32;

// 64 bit interger
typedef long long i64;
typedef unsigned long long u64;

// float types
typedef float  f32; // 32 bit float
typedef double f64; // 64 bit float

static_assert(sizeof(i8) == 1);
static_assert(sizeof(u8) == 1);

static_assert(sizeof(i16) == 2);
static_assert(sizeof(u16) == 2);

static_assert(sizeof(i32) == 4);
static_assert(sizeof(u32) == 4);

static_assert(sizeof(i64) == 8);
static_assert(sizeof(u64) == 8);

static_assert(sizeof(f32) == 4);
static_assert(sizeof(f64) == 8);
