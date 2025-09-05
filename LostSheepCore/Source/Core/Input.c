#include "Input.h"

#include "Core/Window.h"
#include "Log.h"
#include "GLFW/glfw3.h"

int IsKeyPressed(const KeyCode keycode)
{
    GLFWwindow* window = GetNativeWindow();
    int state = glfwGetKey(window, keycode);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

int IsMouseButtonPressed(const MouseCode button)
{
    GLFWwindow* window = GetNativeWindow();
    int state = glfwGetMouseButton(window, button);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

void GetMousePosition(double* xPos, double* yPos)
{
    GLFWwindow* window = GetNativeWindow();
    glfwGetCursorPos(window, xPos, yPos);
}

float GetMouseX()
{
    double xPos = 0.0f;
    GLFWwindow* window = GetNativeWindow();
    glfwGetCursorPos(window, &xPos, NULL);
    return (float)xPos;
}

float GetMouseY()
{
    double yPos = 0.0f;
    GLFWwindow* window = GetNativeWindow();
    glfwGetCursorPos(window, NULL, &yPos);
    return (float)yPos;
}
