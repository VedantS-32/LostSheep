#include "Text.h"

#include "Core/Log.h"

#include "Renderer/Shader.h"

#include "glad/glad.h"

// Thanks to https://learnopengl.com/In-Practice/Text-Rendering

#include "ft2build.h"
#include FT_FREETYPE_H

static const char* s_FontPaths[] = {
    "Content/Font/Karla/static/Karla-Regular.ttf"
    //"Content/Font/Monogram/monogram.ttf"
};

FT_Library s_FT;
FT_Face s_Face = NULL;
static float s_TextSizeBase = 100;
static float s_TextSizeAdj = 4;

static TextCharacter s_Characters[128];

void InitText()
{
    if (FT_Init_FreeType(&s_FT))
    {
        LSH_ERROR("FREETYPE: Could not init FreeType Library");
        return;
    }

    if (FT_New_Face(s_FT, s_FontPaths[0], 0, &s_Face))
    {
        LSH_ERROR("FREETYPE: Failed to load font");
        return;
    }
    else
    {
        LSH_TRACE("Loaded font: %s", s_FontPaths[0]);
    }

    s_TextSizeBase /= s_TextSizeAdj;
    FT_Set_Pixel_Sizes(s_Face, 0, (FT_UInt)s_TextSizeBase);
    //FT_Set_Char_Size(face, 0, 100, 1280, 1280);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        if (FT_Load_Char(s_Face, c, FT_LOAD_RENDER))
        {
            LSH_ERROR("FREETYTPE: Failed to load Glyph");
            continue;
        }

        // generate texture
        uint32_t texture;
        glCreateTextures(GL_TEXTURE_2D, 1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            s_Face->glyph->bitmap.width,
            s_Face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            s_Face->glyph->bitmap.buffer
        );

        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // now store character for later use
        TextCharacter character = {
            (LSHIVec2) { (int)(s_Face->glyph->bitmap.width), (int)(s_Face->glyph->bitmap.rows)},
            (LSHIVec2) { s_Face->glyph->bitmap_left, s_Face->glyph->bitmap_top},
            s_Face->glyph->advance.x,
            texture
        };

        s_Characters[(int)c] = character;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // Undo byte alignment
}

void RenderTextLine(const char* text, uint32_t length, const LSHVec2* position, const LSHVec2* bboxDim, float scale, const LSHVec4* color)
{
    UploadUniform4f("uTextColor", color);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
    glActiveTexture(GL_TEXTURE0);

    float x = position->x + (bboxDim->x * 0.25f);

    scale *= 0.01f * s_TextSizeAdj;

    // iterate through all characters
    for (uint32_t i = 0; i < length; i++)
    {
        TextCharacter ch = s_Characters[(int)(text[i])];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = (position->y + (ch.Size.y - ch.Bearing.y) * scale) + (s_TextSizeBase * scale);

        float w = ch.Size.x * scale;
        float h = -(ch.Size.y * scale);
        // update VBO for each character
        float vertices[] = {
           // Coords             // TexCoords
            xpos + w, ypos,       1.0f, 1.0f, 
            xpos + w, ypos + h,   1.0f, 0.0f, 
            xpos,     ypos + h,   0.0f, 0.0f, 
            xpos,     ypos,       0.0f, 1.0f
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.RendererID);

        // update content of VBO memory
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        // render quad
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)

        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // Undo byte alignment
}

void ShutdownText()
{
    FT_Done_Face(s_Face);
    FT_Done_FreeType(s_FT);

    LSH_TRACE("Shutdown text");
}
