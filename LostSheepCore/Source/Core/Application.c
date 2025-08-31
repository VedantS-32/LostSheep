#include "Application.h"

#define CLAY_IMPLEMENTATION
#pragma warning(push, 0)
#include "clay.h"
#pragma warning(pop)

#include "Core/Window.h"
#include "Core/Log.h"
#include "Event/Event.h"

static int s_Running = 1;

int InitApplication(const char* title, int width, int height)
{
	LSH_INFO("Lost Sheep");
    if (!CreateWindow(title, width, height))
    {
        LSH_FATAL("Can't create application! Window creation failed...");

        return 0;
    }

	SetWindowEventCallback(OnEventApplication);

	LSH_TRACE("Application created");

    return 1;
}

void RunApplication()
{
    while (s_Running)
	{
		OnUpdateWindow();
    }
}

void OnEventApplication(void* event)
{
	Event* e = (Event*)event;
    
	WindowLogEvent(e);
	DispatchEvent(EventTypeWindowClose, e, OnEventWindowClose);

    free(e->Data);
}

int OnEventWindowClose(Event* event)
{
	CloseApplication();
    return 1;
}

void CloseApplication()
{
    LSH_INFO("Closed window: %s", GetWindowData()->Title);
    s_Running = 0;
}

void ShutdownApplication()
{
    LSH_INFO("Application shut down");
}
