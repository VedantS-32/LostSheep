#pragma once
#include <stdint.h>

typedef struct GLFWwindow GLFWwindow;

typedef struct WindowProps
{
	const char* Title;
	int Width;
	int Height;
} WindowProps;

static int InitWindow();

int CreateWindow(const char* title, int width, int height);

const GLFWwindow* GetNativeWindow();

void OnUpdateWindow();

void ShutdownWindow();