#include "Application.h"

#include "Core/Window.h"
#include "Core/Log.h"

#include "Event/Event.h"

#include "Renderer/Renderer.h"

#include <stdlib.h>

static int s_Running = 1;

static float s_LastFrameTime = 0.0f;

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
    
    InitRenderer();

    return 1;
}

void RunApplication()
{
    while (s_Running)
	{
		float deltaTime = GetTimeWindow();
		deltaTime -= s_LastFrameTime;
        s_LastFrameTime = GetTimeWindow();

        deltaTime *= 1000.f;

		BeginRendering();
        OnUpdateRenderer(deltaTime);
        EndRendering();

		OnUpdateWindow(deltaTime);

		//LSH_TRACE("Frame Time: %.3f ms (%.1f FPS)", deltaTime, 1000.0f / deltaTime);
    }
}

void OnEventApplication(Event* event)
{    
	//WindowLogEvent(event);

	DispatchEvent(EventTypeWindowClose, event, OnEventWindowClose);

	OnEventRenderer(event);

    free(event->Data);
}

int OnEventWindowClose(Event* event)
{
	CloseApplication();
    return 1;
}

void CloseApplication()
{
    LSH_INFO("Close window event: %s", GetWindowData()->Title);
    s_Running = 0;
}

void ShutdownApplication()
{
    ShutdownRenderer();
    LSH_INFO("Application shut down");
}
