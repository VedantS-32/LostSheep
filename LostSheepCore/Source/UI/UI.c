#include "UI.h"

#include "Core/Window.h"
#include "Core/Log.h"
#include "Core/Input.h"

#include "Event/Event.h"

#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"

#include "GLFW/glfw3.h"

#define CLAY_IMPLEMENTATION
#pragma warning(push, 0)
#include "clay.h"
#pragma warning(pop)

static float s_DeltaTime = 16.66f;

double s_xPos = 0.0f;
double s_yPos = 0.0f;

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

static void HandleOnClickElement(Clay_ElementId elementId, Clay_PointerData pointerInfo, intptr_t userData)
{
    if(IsMouseButtonPressed(LSH_MOUSE_BUTTON_LEFT))
	    LSH_INFO("Clicked on element, Data: %s", (const char*)userData);
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
        .width = (float)(text.length) * config->fontSize,
            .height = (float)(config->fontSize)
    };
}

static void UpdatePointerState()
{
    Clay_SetPointerState((Clay_Vector2) { GetMouseX(), GetMouseY() }, IsMouseButtonPressed(LSH_MOUSE_BUTTON_LEFT));
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
    glfwGetCursorPos(window,&mouseX, &mouseY);
    Clay_SetPointerState((Clay_Vector2) { (float)mouseX, (float)mouseY }, isMouseDown);

    Clay_UpdateScrollContainers(true, (Clay_Vector2) { 0.0f, 0.0f }, s_DeltaTime);

	LSH_TRACE("UI initialized");
}

void OnUpdateUI(float deltaTime)
{
	s_DeltaTime = deltaTime;

    Clay_BeginLayout();
    BuildUI();
    Clay_RenderCommandArray commands = Clay_EndLayout();

    ProcessRenderUICommands(commands);

    UpdatePointerState();
}

void OnEventUI(Event* event)
{
	DispatchEvent(EventTypeWindowResize, event, OnResizeWindowUI);
	//DispatchEvent(EventTypeMouseMoved, event, OnMouseMoveUI);         // Updating in UpdatePointerState();
	DispatchEvent(EventTypeMouseScrolled, event, OnMouseScrollUI);
	DispatchEvent(EventTypeKeyPressed, event, OnKeyPressUI);
}

void BuildUI()
{
    CLAY({ .id = CLAY_ID("OuterContainer"),
           .backgroundColor = (Clay_Color){0.25f, 0.25f, 0.25f, 1.0f},
        .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = {CLAY_SIZING_GROW(1.0f), CLAY_SIZING_GROW(1.0f)},
                .padding = CLAY_PADDING_ALL(16),
                .childGap = 16
        }
        })
    {
        CLAY({ .id = CLAY_ID("Header") ,
               .backgroundColor = (Clay_Color) { 0.92f, 0.51f, !Clay_Hovered() ? 0.62f : 0.8f, 1.0f },
               .cornerRadius = CLAY_CORNER_RADIUS(8.0f),
			   .border.width = CLAY_BORDER_ALL(Clay_Hovered() ? 2 : 0),
			   .border.color = (Clay_Color){0.8f, 0.8f, 0.8f, 1.0f},
               .layout = {
                   .sizing = {CLAY_SIZING_GROW(1.0f), CLAY_SIZING_PERCENT(0.15f)},
                   .padding = CLAY_PADDING_ALL(16)
            }
            })
            {
            }
        CLAY({ .id = CLAY_ID("Header2") ,
               .backgroundColor = (Clay_Color){0.6f, 0.6f, 1.0f, 1.0f},
               .layout = {
                   .sizing = {CLAY_SIZING_GROW(1.0f), CLAY_SIZING_GROW(1.0f)},
                   .padding = CLAY_PADDING_ALL(16)
                   }
            })
        {
            Clay_OnHover(HandleOnClickElement, "Lost Sheep");
        }
    }

    bool isHovered = Clay_PointerOver(Clay_GetElementId(CLAY_STRING("Header")));
    if (isHovered && IsMouseButtonPressed(LSH_MOUSE_BUTTON_MIDDLE))
    {
        LSH_WARN("Handling input example");
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
	int width = ((int*)event->Data)[0];
	int height = ((int*)event->Data)[1];
    Clay_SetLayoutDimensions((Clay_Dimensions) { (float)width, (float)height });

    return 0;
}

int OnMouseMoveUI(Event* event)
{
    s_xPos = ((double*)event->Data)[0];
    s_yPos = ((double*)event->Data)[1];

    Clay_SetPointerState((Clay_Vector2) { (float)s_xPos, (float)s_yPos }, IsMouseButtonPressed(LSH_MOUSE_BUTTON_LEFT));

    return 1;
}

int OnMouseScrollUI(Event* event)
{
    double xOffset = ((double*)event->Data)[0];
    double yOffset = ((double*)event->Data)[1];
    Clay_UpdateScrollContainers(true, (Clay_Vector2) { (float)xOffset, (float)yOffset }, s_DeltaTime);
    return 1;
}

int OnKeyPressUI(Event* event)
{
    if(*((int*)event->Data) == GLFW_KEY_R)
    {
        RecompileShader("Rectangle.glsl");
        return 1;
	}
    return 0;
}

void ShutdownUI()
{
}
