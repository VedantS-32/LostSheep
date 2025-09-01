#include "UI.h"

#include "Core/Window.h"
#include "Core/Log.h"

#include "Event/Event.h"

#include "Renderer/Renderer.h"

#include "GLFW/glfw3.h"

#define CLAY_IMPLEMENTATION
#pragma warning(push, 0)
#include "clay.h"
#pragma warning(pop)

static const Clay_Color COLOR_LIGHT = { 224, 215, 210, 255 };
static const Clay_Color COLOR_RED = { 168, 66, 28, 255 };
static const Clay_Color COLOR_ORANGE = { 225, 138, 50, 255 };

static float s_DeltaTime = 16.66f;

static const char* ClayCommandTypeToString(Clay_RenderCommandType type) {
    switch (type) {
    case CLAY_RENDER_COMMAND_TYPE_RECTANGLE: return "RECTANGLE";
    case CLAY_RENDER_COMMAND_TYPE_BORDER: return "BORDER";
    case CLAY_RENDER_COMMAND_TYPE_TEXT: return "TEXT";
    case CLAY_RENDER_COMMAND_TYPE_IMAGE: return "IMAGE";
    case CLAY_RENDER_COMMAND_TYPE_SCISSOR_START: return "SCISSOR_START";
    case CLAY_RENDER_COMMAND_TYPE_SCISSOR_END: return "SCISSOR_END";
    case CLAY_RENDER_COMMAND_TYPE_CUSTOM: return "CUSTOM";
    default: return "UNKNOWN";
    }
}

static void HandleClayErrors(Clay_ErrorData errorData)
{
    LSH_ERROR("Type: %s; Msg: %s", ENUM_TO_STRING(errorData.errorType), errorData.errorText.chars);
}

static inline Clay_Dimensions MeasureText(Clay_StringSlice text, Clay_TextElementConfig* config, void* userData) {
    // Clay_TextElementConfig contains members such as fontId, fontSize, letterSpacing etc
    // Note: Clay_String->chars is not guaranteed to be null terminated
	LSH_TRACE("MeasureText: %.2fx%.2f", text.length * config->fontSize, config->fontSize);
    return (Clay_Dimensions) {
        .width = (float)(text.length) * config->fontSize, // <- this will only work for monospace fonts, see the renderers/ directory for more advanced text measurement
            .height = (float)(config->fontSize)
    };
}

void InitUI()
{
	GLFWwindow* window = GetNativeWindow();

    WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);

    uint64_t totalMemorySize = Clay_MinMemorySize();
    Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, malloc(totalMemorySize));
    Clay_Initialize(arena, (Clay_Dimensions) { (float)data->Width, (float)data->Height }, (Clay_ErrorHandler) { HandleClayErrors });
    Clay_SetMeasureTextFunction(MeasureText, 0);

    Clay_SetLayoutDimensions((Clay_Dimensions) { (float)data->Width, (float)data->Height});

    double mouseX, mouseY;
    int isMouseDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    Clay_SetPointerState((Clay_Vector2) { (float)mouseX, (float)mouseY }, isMouseDown);

    Clay_UpdateScrollContainers(true, (Clay_Vector2) { 0.0f, 0.0f }, s_DeltaTime);

	LSH_TRACE("UI initialized");
}

void OnUpdateUI(float deltaTime)
{
	s_DeltaTime = deltaTime;

    // Layout
    Clay_BeginLayout();
    BuildUI();
    Clay_RenderCommandArray commands = Clay_EndLayout();

    ProcessRenderUICommands(commands);
}

void OnEventUI(Event* event)
{
	DispatchEvent(EventTypeWindowResize, event, OnResizeWindowUI);
	DispatchEvent(EventTypeMouseMoved, event, OnMouseMoveUI);
	DispatchEvent(EventTypeMouseScrolled, event, OnMouseScrollUI);
}

void BuildUI()
{
    CLAY({ .id = CLAY_ID("OuterContainer"), .layout = {.sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)}, .padding = CLAY_PADDING_ALL(16), .childGap = 16 }, .backgroundColor = {250,250,255,255} }) {
        CLAY({
            .id = CLAY_ID("SideBar"),
            .layout = {.layoutDirection = CLAY_TOP_TO_BOTTOM, .sizing = {.width = CLAY_SIZING_FIXED(300), .height = CLAY_SIZING_GROW(0) }, .padding = CLAY_PADDING_ALL(16), .childGap = 16 },
            .backgroundColor = COLOR_LIGHT
            })
        {

            CLAY({ .id = CLAY_ID("MainContent"), .layout = {.sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) } }, .backgroundColor = COLOR_LIGHT }) {}
        }
    }
}

void RenderUI()
{
}

void ProcessRenderUICommands(Clay_RenderCommandArray commands)
{
	//LSH_INFO("Processing %d render commands", commands.length);
    for (int i = 0; i < commands.length; i++)
    {
        Clay_RenderCommand* cmd = &commands.internalArray[i];

	    //LSH_TRACE("CommandType: %s", ClayCommandTypeToString(cmd->commandType));

        switch (cmd->commandType)
        {
        case CLAY_RENDER_COMMAND_TYPE_RECTANGLE:
            RenderRectangle(cmd);
            break;
        case CLAY_RENDER_COMMAND_TYPE_BORDER:
            RenderBorder(cmd);
            break;
        case CLAY_RENDER_COMMAND_TYPE_TEXT:
            RenderText(cmd);
            break;
        case CLAY_RENDER_COMMAND_TYPE_IMAGE:
            RenderImage(cmd);
            break;
        case CLAY_RENDER_COMMAND_TYPE_SCISSOR_START:
            StartClipping(cmd);
            break;
        case CLAY_RENDER_COMMAND_TYPE_SCISSOR_END:
            EndClipping(cmd);
            break;
        case CLAY_RENDER_COMMAND_TYPE_CUSTOM:
            RenderCustomElement(cmd);
            break;
        }
    }
}

int OnResizeWindowUI(Event* event)
{
	float width = ((float*)event->Data)[0];
	float height = ((float*)event->Data)[1];
    Clay_SetLayoutDimensions((Clay_Dimensions) { width, height });

    return 1;
}

int OnMouseMoveUI(Event* event)
{
    GLFWwindow* window = GetNativeWindow();

    float xPos = ((float*)event->Data)[0];
    float yPos = ((float*)event->Data)[1];
    int isMouseDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    Clay_SetPointerState((Clay_Vector2) { xPos, yPos }, isMouseDown);

    return 1;
}

int OnMouseScrollUI(Event* event)
{
    float xOffset = ((float*)event->Data)[0];
    float yOffset = ((float*)event->Data)[1];
    Clay_UpdateScrollContainers(true, (Clay_Vector2) { xOffset, yOffset }, s_DeltaTime);
    return 1;
}

void ShutdownUI()
{
}
