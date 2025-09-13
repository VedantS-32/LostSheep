#pragma once

typedef struct LSHVec2
{
	float x;
	float y;
} LSHVec2;

typedef union LSHVec3
{
    struct {
        float r;
        float g;
        float b;
    };
    struct {
        float x;
        float y;
        float z;
    };
} LSHVec3;

// w and a is at the end
typedef union LSHVec4
{
    struct {
        float r;
        float g;
        float b;
        float a;
    };
    struct {
        float x;
        float y;
        float z;
        float w;
    };
} LSHVec4;

typedef struct LSHIVec2
{
	int x;
	int y;
} LSHIVec2;

typedef struct LSHIVec3
{
    int x;
    int y;
    int z;
} LSHIVec3;

// w is at the end
typedef struct LSHIVec4
{
    int x;
    int y;
    int z;
    int w;
} LSHIVec4;