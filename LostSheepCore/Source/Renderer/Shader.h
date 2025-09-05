#pragma once

#include "cglm/types.h"
#include <stdint.h>

typedef struct UniformInfo
{
	char* Name;
	int Location;
} UniformInfo;

typedef struct Shader
{
	char* Name;
	char* Path;

	uint32_t UniformCount;
	UniformInfo Uniforms[16];

	uint32_t RendererID;
} Shader;

void InitShaders();

uint32_t CompileShader(const char* path);
int RecompileShader(const char* name);

void UploadUniform1i(const char* name, int v0);
void UploadUniform2i(const char* name, const ivec2* v0);
void UploadUniform3i(const char* name, const ivec3* v0);
void UploadUniform4i(const char* name, const ivec4* v0);

void UploadUniform1f(const char* name, float v0);
void UploadUniform2f(const char* name, const vec2* v0);
void UploadUniform3f(const char* name, const vec3* v0);
void UploadUniform4f(const char* name, const vec4* v0);

void UploadUniformMat3f(const char* name, const mat3* matrix);
void UploadUniformMat4f(const char* name, const mat4* matrix);

void ShutdownShaders();