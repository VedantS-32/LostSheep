#pragma once

#include "Math/Types.h"

#include <stdint.h>

typedef struct TextCharacter {
    LSHIVec2   Size;       // Size of glyph
    LSHIVec2   Bearing;    // Offset from baseline to left/top of glyph
    uint32_t Advance;    // Offset to advance to next glyph
    uint32_t RendererID;  // ID handle of the glyph texture
} TextCharacter;


void InitText();

void RenderTextLine(const char* text, uint32_t length, const LSHVec2* position, const LSHVec2* bboxDim, float scale, const LSHVec4* color);

void ShutdownText();