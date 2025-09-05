#pragma once

#include "Core/KeyCode.h"
#include "Core/MouseButtonCode.h"

typedef float vec2[2];

int IsKeyPressed(const KeyCode keycode);

int IsMouseButtonPressed(const MouseCode button);
void GetMousePosition(double* xPos, double* yPos);
float GetMouseX();
float GetMouseY();