#pragma once

typedef struct Event Event;
typedef struct Clay_RenderCommandArray Clay_RenderCommandArray;	

void InitUI();

void OnUpdateUI(float deltaTime);

void OnEventUI(Event* event);

void BuildUI();

void RenderUI();

void ProcessRenderUICommands(Clay_RenderCommandArray commands);

int OnResizeWindowUI(Event* event);

int OnMouseMoveUI(Event* event);

int OnMouseScrollUI(Event* event);

int OnKeyPressUI(Event* event);

int OnMouseClickedUI(Event* event);

void ShutdownUI();