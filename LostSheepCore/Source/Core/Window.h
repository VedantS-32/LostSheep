#pragma once
#include <stdint.h>

typedef struct GLFWwindow GLFWwindow;

typedef struct Event Event;

typedef void (* EventCallbackHandlefn)(void* Event);

typedef struct WindowData
{
	const char* Title;
	int Width;
	int Height;
	EventCallbackHandlefn EventCallback;
} WindowData;

static int InitWindow();

// Set GLFW callbacks
static void RegisterCallbacks(GLFWwindow* window);

int CreateWindow(const char* title, int width, int height);

const GLFWwindow* GetNativeWindow();

const WindowData* GetWindowData();

void OnUpdateWindow();

void ShutdownWindow();

void WindowLogEvent(Event* event);

// Events
void SetWindowEventCallback(EventCallbackHandlefn callback);

void WindowResizeCallback(GLFWwindow* window, int width, int height);

void WindowCloseCallback(GLFWwindow* window);

void WindowPositionCallback(GLFWwindow* window, int xPos, int yPos);

void WindowKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void WindowCharacterCallback(GLFWwindow* window, unsigned int keycode);

void WindowMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

void WindowCursorPositionCallback(GLFWwindow* window, double xPos, double yPos);

void WindowScrollCallback(GLFWwindow* window, double xOffset, double yOffset);