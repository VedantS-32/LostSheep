#pragma once

typedef struct Event Event;
typedef struct Clay_RenderCommandArray Clay_RenderCommandArray;

typedef void (*TabUIfn)(const char* tabName);

typedef struct TabContent
{
	TabUIfn RenderTabUI;
} TabContent;

typedef struct TabBarElement
{
	int TabIndex;
	char* TabName;
	TabContent Content;
} TabBarElement;

typedef struct TabBarContent
{
	int Count;
	int Capacity;
	TabBarElement** TabBarElements;
} TabBarContent;

// TabBar

// Initializes TabBar
void InitTabBarContent();

// Dynamically adds new TabBar
void AddTabBarElement(const char* tabName, TabUIfn UIfn);

// Cleans TabBar content
void CleanTabBarContent();

// Main UI
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