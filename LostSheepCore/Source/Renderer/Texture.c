#include "Texture.h"

#include "Core/Log.h"

#include "glad/glad.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static TextureInfo* s_Textures[64];
static uint32_t s_TextureCount = 0;

static const s_TexturePathCount = 2;
static const char* s_TexturePaths[] = {
	"Content/Texture/CStell.png",
	"Content/Texture/UVChecker.png"
};

static GLenum ToOpenGLTexInternalFormat(ImageFormat format)
{
	switch (format)
	{
	case ImageFormat_None:
		LSH_ERROR("Invalid format");
		break;
	case ImageFormat_R8:
		LSH_WARN("Not implemented yet");
		break;
	case ImageFormat_RGB8:
		return GL_RGB8;
		break;
	case ImageFormat_RGBA8:
		return GL_RGBA8;
		break;
	case ImageFormat_RGBA16F:
		return GL_RGBA16F;
		break;
	case ImageFormat_RGBA32F:
		return GL_RGBA32F;
		break;
	default:
		LSH_ERROR("Invalid format");
		return GL_NONE;
		break;
	}

	return GL_NONE;
}

static GLenum ToOpenGLTexDataFormat(ImageFormat format)
{
	switch (format)
	{
	case ImageFormat_None:
		LSH_ERROR("Invalid format");
		break;
	case ImageFormat_R8:
		LSH_WARN("Not implemented yet");
		break;
	case ImageFormat_RGB8:
		return GL_RGB;
		break;
	case ImageFormat_RGBA8:
		return GL_RGBA;
		break;
	case ImageFormat_RGBA16F:
		return GL_RGBA;
		break;
	case ImageFormat_RGBA32F:
		return GL_RGBA;
		break;
	default:
		LSH_ERROR("Invalid format");
		return GL_NONE;
		break;
	}

	return GL_NONE;
}

static uint32_t GetTextureIDByName(const char* name)
{
	for (uint32_t i = 0; i < s_TextureCount; i++)
	{
		if (strcmp(s_Textures[s_TextureCount]->Name, name) == 0)
		{
			return s_Textures[s_TextureCount]->RendererID;
		}
	}
	return -1;
}

void InitTexture()
{
	for (uint32_t i = 0; i < s_TexturePathCount; i++)
	{
		LoadTexture(s_TexturePaths[i]);
	}
}

int LoadTexture(const char* path)
{
	int width, height, channels;
	const char* name;
	stbi_uc* data = NULL;

	{
		data = stbi_load(path, &width, &height, &channels, 0);
		name = strrchr(path, '/');
	}
	if (!data)
	{
		data = stbi_load("Content/Texture/UVChecker.png", &width, &height, &channels, 0);
		name = "DefaultTexture";
	}

	TextureSpecification spec;
	spec.Width = width;
	spec.Height = height;

	if (channels == 4)
	{
		spec.Format = ImageFormat_RGBA8;
	}
	else if (channels == 3)
	{
		spec.Format = ImageFormat_RGB8;
	}

	TextureInfo* texture = CreateTexture(&spec, data);
	texture->Name = name;
	texture->Path = path;
	stbi_image_free(data);

	s_Textures[s_TextureCount] = texture;
	s_TextureCount++;

	LSH_TRACE("Imported Texture2D; Path: %s", path);

	return texture->RendererID;
}

uint32_t GetTextureRendererID(TextureName textureName)
{
	return s_Textures[textureName]->RendererID;
}

TextureInfo* CreateTexture(const TextureSpecification* spec, const void* data)
{
	TextureInfo* texture = (TextureInfo*)malloc(sizeof(TextureInfo));
	uint32_t internalFormat = ToOpenGLTexInternalFormat(spec->Format);
	uint32_t dataFormat = ToOpenGLTexDataFormat(spec->Format);
	uint32_t rendererID = 0;

	glCreateTextures(GL_TEXTURE_2D, 1, &rendererID);
	glBindTexture(GL_TEXTURE_2D, rendererID);
	glTextureStorage2D(rendererID, 1, internalFormat, spec->Width, spec->Height);

	glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(rendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTextureSubImage2D(rendererID, 0, 0, 0, spec->Width, spec->Height, dataFormat, GL_UNSIGNED_BYTE, data);

	texture->RendererID = rendererID;
	texture->InternalFormat = internalFormat;
	texture->DataFormat = dataFormat;
	texture->Width = spec->Width;
	texture->Height = spec->Height;

	return texture;
}

void BindActiveTexture(TextureName textureName, uint32_t slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, GetTextureRendererID(textureName));
}

void UnbindTexture(const char* name)
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ShutdownTexture()
{
	for (uint32_t i = 0; i < s_TextureCount; i++)
	{
		glDeleteTextures(1, &(s_Textures[i]->RendererID));

		free(s_Textures[i]);
	}

	LSH_TRACE("Shutdown texture");
}
