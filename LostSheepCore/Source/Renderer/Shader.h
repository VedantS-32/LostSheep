#pragma once

#include "Math/Types.h"

#include "cglm/cglm.h"

#include <stdint.h>

typedef enum UIShaderType
{
	UIShaderType_Rectangle = 0,
	UIShaderType_Image,
	UIShaderType_Text,

} UIShaderType;

typedef struct UniformInfo
{
	char* Name;
	int Location;
} UniformInfo;

typedef struct Shader
{
	char* Name;
	char* Path;
	UIShaderType uiShaderType;

	uint32_t UniformCount;
	UniformInfo Uniforms[16];

	uint32_t RendererID;
} Shader;

void InitShader();

uint32_t CompileShader(const char* path);
int RecompileShader(const char* name);

void SetActiveShader(UIShaderType uiShaderType);

void UploadUniform1i(const char* name, int v0);
void UploadUniform2i(const char* name, const LSHIVec2* v0);
void UploadUniform3i(const char* name, const LSHIVec3* v0);
void UploadUniform4i(const char* name, const LSHIVec4* v0);

void UploadUniform1f(const char* name, float v0);
void UploadUniform2f(const char* name, const LSHVec2* v0);
void UploadUniform3f(const char* name, const LSHVec3* v0);
void UploadUniform4f(const char* name, const LSHVec4* v0);

void UploadUniformMat3f(const char* name, const mat3* matrix);
void UploadUniformMat4f(const char* name, const mat4* matrix);

void ShutdownShader();