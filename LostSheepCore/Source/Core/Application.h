#pragma once

typedef struct Event Event;

int InitApplication(const char* title, int width, int height);

void RunApplication();

void OnEventApplication(Event* event);

int OnEventWindowClose(Event* event);

void CloseApplication();

void ShutdownApplication();