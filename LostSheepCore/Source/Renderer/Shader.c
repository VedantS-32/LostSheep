#include "Shader.h"

#include "Core/Log.h"

#include "glad/glad.h"

#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

static Shader* s_Shaders[16];
static uint32_t s_ShadersCount = 0;

static Shader* s_ActiveShader = NULL;

// Must be in the serial of Rectangle, Image, Text; as UIShaderType enum
static const char* s_ShadersPaths[] = {
	"Content/Shader/Rectangle.glsl",
	"Content/Shader/Texture.glsl"
};

static uint32_t s_ShaderPathCount = 2;

static char* ReadFile(const char* path)
{
	char* buffer = NULL;
	size_t string_size = 0, read_size = 0;
	FILE* handler = fopen(path, "rb");

	if (handler)
	{
		fseek(handler, 0, SEEK_END);
		string_size = (size_t)ftell(handler);
		rewind(handler);

		if (string_size > 0)
		{
			buffer = (char*)malloc(sizeof(char) * (string_size + 1));
			if (buffer != NULL)
			{
				read_size = (size_t)fread(buffer, sizeof(char), string_size, handler);
				buffer[string_size] = '\0';

				if(read_size != string_size)
				{
					LSH_WARN("Could not read the entire file: %s", path);
					free(buffer);
					buffer = NULL;
				}
			}
		}

		fclose(handler);
	}

	return buffer;
}

static Shader* GetShaderByUIShaderType(UIShaderType uiShaderType)
{
	return s_Shaders[uiShaderType];
}

static Shader* GetShaderByName(const char* name)
{
	for(uint32_t i = 0; i < s_ShadersCount; i++)
	{
		if (s_Shaders[i] == NULL)
			continue;
		if (strcmp(s_Shaders[i]->Name, name) == 0)
			return s_Shaders[i];
	}

	LSH_WARN("Could not find shader: %s", name);

	return NULL;
}

static int GetUniformLocation(const char* name)
{
	uint32_t* uniformCount = &(s_ActiveShader->UniformCount);
	UniformInfo* uniforms = s_ActiveShader->Uniforms;
	for(uint32_t i = 0; i < *uniformCount; i++)
	{
		if (strcmp(uniforms[i].Name, name) == 0)
		{
			return uniforms[i].Location;
		}
	}

	int location = glGetUniformLocation(s_ActiveShader->RendererID, name);
	if (location == -1)
	{
		LSH_WARN("Could not find uniform: %s", name);
		return -1;
	}

	if (*uniformCount >= 16)
	{
		LSH_FATAL("Too many uniforms, increase the size of the Shader.Uniforms array");
		return -1;
	}

	UniformInfo* uniform = &(uniforms[*uniformCount]);
	uniform->Location = location;
	uniform->Name = _strdup(name);

	if (uniform->Name == NULL)
	{
		LSH_FATAL("Failed to allocate memory for UniformInfo Name");
		return -1;
	}

	(*uniformCount)++;

	return location;
}

static void ParseShader(const char* path, char** vertexSource, char** fragmentSource)
{
	const char* source = ReadFile(path);

	const char* vertexToken = "#shader vertex";
	const char* fragmentToken = "#shader fragment";
	const char* vertexTokenLocation = strstr(source, vertexToken);
	const char* fragmentTokenLocation = strstr(source, fragmentToken);
	if (vertexTokenLocation == NULL || fragmentTokenLocation == NULL)
	{
		LSH_FATAL("Could not find shader tokens");
		if (source)
			free((void*)source);
		*vertexSource = NULL;
		*fragmentSource = NULL;
		return;
	}
	size_t vertexSourceLength = fragmentTokenLocation - (vertexTokenLocation + strlen(vertexToken));
	size_t fragmentSourceLength = strlen(source) - (fragmentTokenLocation - source) - strlen(fragmentToken);
	*vertexSource = (char*)malloc(vertexSourceLength + 1);
	*fragmentSource = (char*)malloc(fragmentSourceLength + 1);
	if (*vertexSource != NULL)
	{
		memcpy(*vertexSource, vertexTokenLocation + strlen(vertexToken), vertexSourceLength);
		(*vertexSource)[vertexSourceLength] = '\0';
	}
	if (*fragmentSource != NULL)
	{
		memcpy(*fragmentSource, fragmentTokenLocation + strlen(fragmentToken), fragmentSourceLength);
		(*fragmentSource)[fragmentSourceLength] = '\0';
	}

	free((void*)source);
}

static void InvalidateUniforms(const char* shaderName)
{
	Shader* shader = GetShaderByName(shaderName);
	if (shader == NULL)
		return;

	for(uint32_t i = 0; i < shader->UniformCount; i++)
	{
		free(shader->Uniforms[i].Name);
	}
	shader->UniformCount = 0;
}

void InitShader()
{
	for (uint32_t i = 0; i < s_ShaderPathCount; i++)
	{
		uint32_t rendererID = CompileShader(s_ShadersPaths[i]);

		if (rendererID <= 0)
		{
			LSH_FATAL("Failed to compile shader: %s", s_ShadersPaths[i]);
			continue;
		}
		Shader* shader = (Shader*)malloc(sizeof(Shader));
		if (shader == NULL)
		{
			LSH_FATAL("Failed to allocate memory for Shader");
			continue;
		}
		memset(shader, 0, sizeof(Shader));
		const char* path = s_ShadersPaths[i];
		const char* name = strrchr(path, '/');
		if (name == NULL)
			name = path;
		else
			name++; // Skip the '/'
		shader->Name = _strdup(name);
		shader->Path = _strdup(path);
		shader->uiShaderType = (UIShaderType)i;
		shader->RendererID = rendererID;
		shader->UniformCount = 0;
		memset(shader->Uniforms, 0, sizeof(shader->Uniforms));
		s_Shaders[s_ShadersCount] = shader;

		s_ShadersCount++;

		LSH_TRACE("Shader program compiled; Path: %s", path);
	}

	s_ActiveShader = s_Shaders[0];
	glUseProgram(s_ActiveShader->RendererID);
}

uint32_t CompileShader(const char* path)
{
	char* vertexSource = NULL;
	char* fragmentSource = NULL;

	uint32_t vertexShader;
	uint32_t fragmentShader;

	ParseShader(path, &vertexSource, &fragmentSource);

	//LSH_TRACE("Vertex Shader Source:\n%s", vertexSource);
	//LSH_TRACE("Fragment Shader Source:\n%s", fragmentSource);

	if (vertexSource == NULL || fragmentSource == NULL)
	{
		LSH_FATAL("Failed to load shader sources");
		return 0;
	}

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const char* const*)&vertexSource, NULL);
	glCompileShader(vertexShader);

	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		LSH_ERROR("SHADER::VERTEX::COMPILATION_FAILED, %s", infoLog);

		return 0;
	}

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const char* const*)&fragmentSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		LSH_ERROR("SHADER::FRAGMENT::COMPILATION_FAILED, %s", infoLog);

		return 0;
	}

	uint32_t shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glUseProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	free(vertexSource);
	free(fragmentSource);

	return shaderProgram;
}

int RecompileShader(const char* name)
{
	Shader* shader = GetShaderByName(name);
	if (shader == NULL)
	{
		LSH_ERROR("Could not find shader to recompile: %s", name);
		return 0;
	}
	uint32_t newRendererID = CompileShader(shader->Path);
	if (newRendererID == 0)
	{
		LSH_ERROR("Failed to recompile shader: %s", name);
		return 0;
	}
	shader->RendererID = newRendererID;
	return 1;
}

void SetActiveShader(UIShaderType uiShaderType)
{
	if(s_ActiveShader->uiShaderType != uiShaderType)
	{
		Shader* shader = GetShaderByUIShaderType(uiShaderType);
		glUseProgram(shader->RendererID);
		s_ActiveShader = shader;
	}
}

void UploadUniform1i(const char* name, int value)
{
    int location = GetUniformLocation(name);
    if (location != -1)
    {
        glUniform1i(location, (GLint)value);
    }
}

void UploadUniform2i(const char* name, const ivec2* value)
{
    int location = GetUniformLocation(name);
	int* valuePtr = (int*)value;
    if (location != -1)
    {
        glUniform2i(location, (GLint)(valuePtr[0]), (GLint)(valuePtr[1]));
    }
}

void UploadUniform3i(const char* name, const ivec3* value)
{
    int location = GetUniformLocation(name);
	int* valuePtr = (int*)value;
    if (location != -1)
    {
        glUniform3i(location, (GLint)(valuePtr[0]), (GLint)(valuePtr[1]), (GLint)(valuePtr[2]));
    }
}

void UploadUniform4i(const char* name, const ivec4* value)
{
	int location = GetUniformLocation(name);
	int* valuePtr = (int*)value;
	if (location != -1)
	{
		glUniform4i(location, (GLint)(valuePtr[0]), (GLint)(valuePtr[1]), (GLint)(valuePtr[2]), (GLint)(valuePtr[3]));
	}
}

void UploadUniform1f(const char* name, float value)
{
	int location = GetUniformLocation(name);
	if (location != -1)
	{
		glUniform1f(location, (GLfloat)value);
	}
}

void UploadUniform2f(const char* name, const vec2* value)
{
	int location = GetUniformLocation(name);
	float* valuePtr = (float*)value;
	if (location != -1)
	{
		glUniform2f(location, (GLfloat)(valuePtr[0]), (GLfloat)(valuePtr[1]));
	}
}

void UploadUniform3f(const char* name, const vec3* value)
{
	int location = GetUniformLocation(name);
	float* valuePtr = (float*)value;
	if (location != -1)
	{
		glUniform3f(location, (GLfloat)(valuePtr[0]), (GLfloat)(valuePtr[1]), (GLfloat)(valuePtr[2]));
	}
}

void UploadUniform4f(const char* name, const vec4* value)
{
	int location = GetUniformLocation(name);
	float* valuePtr = (float*)value;
	if (location != -1)
	{
		glUniform4f(location, (GLfloat)(valuePtr[0]), (GLfloat)(valuePtr[1]), (GLfloat)(valuePtr[2]), (GLfloat)(valuePtr[3]));
	}
}

void UploadUniformMat3f(const char* name, const mat3* matrix)
{
	int location = GetUniformLocation(name);
	if (location != -1)
	{
		glUniformMatrix3fv(location, 1, GL_FALSE, (const GLfloat*)matrix);
	}
}

void UploadUniformMat4f(const char* name, const mat4* matrix)
{
	int location = GetUniformLocation(name);
	if (location != -1)
	{
		glUniformMatrix4fv(location, 1, GL_FALSE, (const GLfloat*)matrix);
	}
}

void ShutdownShader()
{
	for(uint32_t i = 0; i < s_ShadersCount; i++)
	{
		if (s_Shaders[i] == NULL)
			continue;
		glDeleteProgram(s_Shaders[i]->RendererID);
		free(s_Shaders[i]->Name);
		free(s_Shaders[i]->Path);
		for(uint32_t j = 0; j < s_Shaders[i]->UniformCount; j++)
		{
			free(s_Shaders[i]->Uniforms[j].Name);
		}
		free(s_Shaders[i]);
	}

	LSH_TRACE("Shutdown shader");
}
