#pragma once

#include <stdint.h>

typedef enum TextureName
{
	TextureName_CStell,
	TextureName_UVChecker,
	TextureName_Minimize,
	TextureName_Maximize,
	TextureName_Close
} TextureName;

typedef enum ImageFormat
{
	ImageFormat_None = 0,
	ImageFormat_R8,
	ImageFormat_RGB8,
	ImageFormat_RGBA8,
	ImageFormat_RGBA16F,
	ImageFormat_RGBA32F
} ImageFormat;

typedef struct TextureSpecification
{
	uint32_t Width;
	uint32_t Height;
	ImageFormat Format;
} TextureSpecification;

typedef struct TextureInfo
{
	char* Name;
	char* Path;
	uint32_t Width;
	uint32_t Height;
	uint32_t InternalFormat;
	uint32_t DataFormat;
	uint32_t RendererID;
	int GenerateMips;
} TextureInfo;

void InitTexture();

uint32_t LoadTexture(const char* path);

uint32_t GetTextureRendererID(TextureName textureName);

TextureInfo* CreateTexture(const TextureSpecification* spec, const void* data);

void BindActiveTexture(TextureName textureName, uint32_t slot);

void UnbindTexture(const char* name);

void ShutdownTexture();