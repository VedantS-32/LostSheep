#include "Core/Application.h"

int main()
{
    if (!InitApplication("Lost Sheep", 1280, 720))
    {
        return -1;
    }

    RunApplication();

    ShutdownApplication();
}