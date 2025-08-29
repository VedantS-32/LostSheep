#include "Window.h"

#include "Log.h"

#define GLAD_GL_IMPLEMENTATION
#include "glad/glad.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

static GLFWwindow* s_Window;
static WindowProps s_WindowProps;

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

int CreateWindow(const char* title, int width, int height)
{
	if (!InitWindow())
	{
		return 0;
	}

	s_WindowProps.Title = title;
	s_WindowProps.Width = width;
	s_WindowProps.Height = height;

	s_Window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (!s_Window)
	{
		LSH_FATAL("Failed to create window!");
		glfwTerminate();
		return 0;
	}

	LSH_TRACE("Window created: %s (%dx%d)", title, width, height);

	glfwMakeContextCurrent(s_Window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	return 1;
}

const GLFWwindow* GetNativeWindow()
{
	return s_Window;
}

void OnUpdateWindow()
{
	glfwGetFramebufferSize(s_Window, &s_WindowProps.Width, &s_WindowProps.Height);
	glViewport(0, 0, s_WindowProps.Width, s_WindowProps.Height);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(1.00f, 0.51f, 0.65f, 1.0f);

	glfwSwapBuffers(s_Window);

	glfwPollEvents();
}

void ShutdownWindow()
{
	glfwTerminate();
	LSH_TRACE("GLFW terminated");
}
