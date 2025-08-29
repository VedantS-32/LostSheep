#include "Application.h"

#define CLAY_IMPLEMENTATION
#pragma warning(push, 0)
#include "clay.h"
#pragma warning(pop)


#include "Window.h"
#include "Log.h"

static int s_Running = 1;

int InitApplication(const char* title, int width, int height)
{
	LSH_INFO("Lost Sheep");
    if (!CreateWindow(title, width, height))
    {
        LSH_FATAL("Can't create application! Window creation failed...");

        return 0;
    }

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

void CloseApplication()
{
    s_Running = 0;
}

void ShutdownApplication()
{
    LSH_TRACE("Application shut down");
}
