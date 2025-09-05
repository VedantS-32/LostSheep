#include "Shader.h"

#include "Core/Log.h"

#include "glad/glad.h"

#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

static UniformInfo* s_Uniforms[32];
static int s_UniformsCount = 0;
static unsigned int shaderProgram;

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

static int GetUniformLocation(const char* name)
{
	for(int i = 0; i < s_UniformsCount; i++)
	{
		if (strcmp(s_Uniforms[i]->Name, name) == 0)
		{
			return s_Uniforms[i]->Location;
		}
	}

	int location = glGetUniformLocation(shaderProgram, name);
	if (location == -1)
	{
		LSH_WARN("Could not find uniform: %s", name);
		return -1;
	}

	if (s_UniformsCount >= 32)
	{
		LSH_FATAL("Too many uniforms, increase the size of the s_Uniforms array");
		return -1;
	}
    UniformInfo* info = (UniformInfo*)(malloc(sizeof(UniformInfo)));
    if (info == NULL)
    {
        LSH_FATAL("Failed to allocate memory for UniformInfo");
        return -1;
    }
    info->Name = _strdup(name);
    if (info->Name == NULL)
    {
        LSH_FATAL("Failed to allocate memory for UniformInfo Name");
        free(info);
        return -1;
    }

	info->Location = location;
	s_Uniforms[s_UniformsCount] = info;
	s_UniformsCount++;

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

static void InvalidateUniforms()
{
	for(int i = 0; i < s_UniformsCount; i++)
	{
		free(s_Uniforms[i]->Name);
		free(s_Uniforms[i]);
	}
	s_UniformsCount = 0;
}

void InitShaders()
{
	CompileShaders();
}

void CompileShaders()
{
	if(shaderProgram != 0)
	{
		glDeleteProgram(shaderProgram);
		InvalidateUniforms();
	}

	char* vertexSource = NULL;
	char* fragmentSource = NULL;

	const char* path = "Content/Shader/Rectangle.glsl";

	uint32_t vertexShader;
	uint32_t fragmentShader;

	ParseShader(path, &vertexSource, &fragmentSource);

	//LSH_TRACE("Vertex Shader Source:\n%s", vertexSource);
	//LSH_TRACE("Fragment Shader Source:\n%s", fragmentSource);

	if (vertexSource == NULL || fragmentSource == NULL)
	{
		LSH_FATAL("Failed to load shader sources");
		return;
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
	}

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const char* const*)&fragmentSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		LSH_ERROR("SHADER::FRAGMENT::COMPILATION_FAILED, %s", infoLog);
	}

	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glUseProgram(shaderProgram);

	LSH_TRACE("Shader program compiled and linked successfully");

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	free(vertexSource);
	free(fragmentSource);
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

void ShutdownShaders()
{
	for (int i = 0; i < s_UniformsCount; i++)
	{
		free(s_Uniforms[i]->Name);
		free(s_Uniforms[i]);
	}
	s_UniformsCount = 0;
}
