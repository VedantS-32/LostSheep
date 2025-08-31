#include "Window.h"

#include "Core/Log.h"
#include "Event/Event.h"

#define GLAD_GL_IMPLEMENTATION
#include "glad/glad.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "cglm/cglm.h"

#include <stdlib.h>

static GLFWwindow* s_WindowHandle;
static WindowData s_WindowData;

int InitWindow()
{
	if (!glfwInit())
	{
		LSH_FATAL("Failed to initialize GLFW!");
		return 0;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwSwapInterval(1);

	LSH_TRACE("GLFW initialized");

	return 1;
}

void RegisterCallbacks(GLFWwindow* window)
{
	glfwSetWindowUserPointer(window, &s_WindowData);

	glfwSetWindowSizeCallback(window, WindowResizeCallback);

	glfwSetWindowCloseCallback(window, WindowCloseCallback);

	glfwSetWindowPosCallback(window, WindowPositionCallback);

	glfwSetKeyCallback(window, WindowKeyCallback);

	glfwSetCharCallback(window, WindowCharacterCallback);

	glfwSetMouseButtonCallback(window, WindowMouseButtonCallback);

	glfwSetCursorPosCallback(window, WindowCursorPositionCallback);

	glfwSetScrollCallback(window, WindowScrollCallback);
}

int CreateWindow(const char* title, int width, int height)
{
	if (!InitWindow())
	{
		return 0;
	}

	s_WindowData.Title = title;
	s_WindowData.Width = width;
	s_WindowData.Height = height;

	s_WindowHandle = glfwCreateWindow(width, height, title, NULL, NULL);
	if (!s_WindowHandle)
	{
		LSH_FATAL("Failed to create window!");
		glfwTerminate();
		return 0;
	}

	LSH_TRACE("Window created: %s (%dx%d)", title, width, height);

	glfwMakeContextCurrent(s_WindowHandle);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	RegisterCallbacks(s_WindowHandle);

	return 1;
}

const GLFWwindow* GetNativeWindow()
{
	return s_WindowHandle;
}

const WindowData* GetWindowData()
{
	return &s_WindowData;
}

void OnUpdateWindow()
{
	glfwGetFramebufferSize(s_WindowHandle, &s_WindowData.Width, &s_WindowData.Height);
	glViewport(0, 0, s_WindowData.Width, s_WindowData.Height);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(1.00f, 0.51f, 0.65f, 1.0f);

	glfwSwapBuffers(s_WindowHandle);

	glfwPollEvents();
}

void ShutdownWindow()
{
	glfwTerminate();
	LSH_TRACE("GLFW terminated");
}

void WindowLogEvent(Event* event)
{
	switch (event->Type)
	{
	case EventTypeWindowClose:
		LSH_TRACE("Event: Window Close");
		break;
	case EventTypeWindowResize:
		LSH_TRACE("Event: Window Resize to %dx%d", ((int*)event->Data)[0], ((int*)event->Data)[1]);
		break;
	case EventTypeWindowMove:
		LSH_TRACE("Event: Window Move to x:%.d, y:%d", ((int*)event->Data)[0], ((int*)event->Data)[1]);
		break;
	case EventTypeKeyPressed:
		LSH_TRACE("Event: Key Pressed %d (scancode: %d)", ((int*)event->Data)[0], ((int*)event->Data)[1]);
		break;
	case EventTypeKeyReleased:
		LSH_TRACE("Event: Key Released %d (scancode: %d)", ((int*)event->Data)[0], ((int*)event->Data)[1]);
		break;
	case EventTypeKeyRepeat:
		LSH_TRACE("Event: Key Repeat %d (scancode: %d)", ((int*)event->Data)[0], ((int*)event->Data)[1]);
		break;
	case EventTypeKeyTyped:
		LSH_TRACE("Event: Key Typed %d", ((unsigned int*)event->Data)[0]);
		break;
	case EventTypeMouseButtonPressed:
		LSH_TRACE("Event: Mouse Button Pressed %d (scancode: %d)", ((int*)event->Data)[0], ((int*)event->Data)[1]);
		break;
	case EventTypeMouseButtonReleased:
		LSH_TRACE("Event: Mouse Button Released %d (scancode: %d)", ((int*)event->Data)[0], ((int*)event->Data)[1]);
		break;
	case EventTypeMouseButtonRepeat:
		LSH_TRACE("Event: Mouse Button Repeat %d (scancode: %d)", ((int*)event->Data)[0], ((int*)event->Data)[1]);
		break;
	case EventTypeMouseMoved:
		LSH_TRACE("Event: Mouse Moved to x:%.2f, y:%.2f", ((double*)event->Data)[0], ((double*)event->Data)[1]);
		break;
	case EventTypeMouseScrolled:
		LSH_TRACE("Event: Mouse Scrolled to %.2f,%.2f", ((double*)event->Data)[0], ((double*)event->Data)[1]);
		break;
	default:
		break;
	}
}

void SetWindowEventCallback(EventCallbackHandlefn callback)
{
	s_WindowData.EventCallback = callback;
}

void WindowResizeCallback(GLFWwindow* window, int width, int height)
{
	WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);
	data->Width = width;
	data->Height = height;

	Event event;
	event.Type = EventTypeWindowResize;
	event.Data = (void*)malloc(sizeof(ivec2));
	glm_ivec2_copy((ivec2){ width, height }, *(ivec2*)event.Data);
	event.Size = sizeof(ivec2);

	data->EventCallback(&event);
}

void WindowCloseCallback(GLFWwindow* window)
{
	WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);

	Event event;
	event.Type = EventTypeWindowClose;
	event.Data = NULL;
	event.Size = 0;

	data->EventCallback(&event);
}

void WindowPositionCallback(GLFWwindow* window, int xPos, int yPos)
{
	WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);

	Event event;
	event.Type = EventTypeWindowMove;
	event.Data = (void*)malloc(sizeof(ivec2));
	glm_ivec2_copy((ivec2) { xPos, yPos }, * (ivec2*)event.Data);
	event.Size = sizeof(ivec2);

	data->EventCallback(&event);
}

void WindowKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);

	Event event;

	switch (action)
	{
	case GLFW_PRESS:
		event.Type = EventTypeKeyPressed;
		break;
	case GLFW_RELEASE:
		event.Type = EventTypeKeyReleased;
		break;
	case GLFW_REPEAT:
		event.Type = EventTypeKeyRepeat;
	}

	event.Data = (void*)malloc(3 * sizeof(int));
	((int*)event.Data)[0] = key;
	((int*)event.Data)[1] = scancode;
	((int*)event.Data)[2] = action;
	event.Size = 3 * sizeof(int);

	data->EventCallback(&event);
}

void WindowCharacterCallback(GLFWwindow* window, unsigned int keycode)
{
	WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);

	Event event;
	event.Type = EventTypeKeyTyped;
	event.Data = (void*)malloc(sizeof(unsigned int));
	((unsigned int*)event.Data)[0] = keycode;
	event.Size = sizeof(unsigned int);

	data->EventCallback(&event);
}

void WindowMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);

	Event event;

	switch (action)
	{
	case GLFW_PRESS:
		event.Type = EventTypeMouseButtonPressed;
		break;
	case GLFW_RELEASE:
		event.Type = EventTypeMouseButtonReleased;
		break;
	case GLFW_REPEAT:
		event.Type = EventTypeMouseButtonRepeat;
	}

	event.Data = (void*)malloc(2 * sizeof(int));
	((int*)event.Data)[0] = button;
	((int*)event.Data)[1] = action;
	event.Size = 2 * sizeof(int);

	data->EventCallback(&event);
}

void WindowCursorPositionCallback(GLFWwindow* window, double xPos, double yPos)
{
	WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);

	Event event;
	event.Type = EventTypeMouseMoved;
	event.Data = (void*)malloc(2 * sizeof(double));
	((double*)event.Data)[0] = xPos;
	((double*)event.Data)[1] = yPos;
	event.Size = 2 * sizeof(double);

	data->EventCallback(&event);
}

void WindowScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);

	Event event;
	event.Type = EventTypeMouseScrolled;
	event.Data = (void*)malloc(2 * sizeof(double));
	((double*)event.Data)[0] = xOffset;
	((double*)event.Data)[1] = yOffset;
	event.Size = 2 * sizeof(double);

	data->EventCallback(&event);
}
