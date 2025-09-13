#include "UI.h"

#include "Core/Application.h"
#include "Core/Window.h"
#include "Core/Log.h"
#include "Core/Input.h"

#include "Event/Event.h"

#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"

#include "GLFW/glfw3.h"

#define CLAY_IMPLEMENTATION
#pragma warning(push, 0)
#include "clay.h"
#pragma warning(pop)

static int s_DebugLayout = 0;

static float s_DeltaTime = 16.66f;

static float s_DoubleClickThreshold = 510.0f;
static int s_StartDoubleClickEvent = 0;
static float s_DoubleClickDelta = 0.0f;

static double s_xPos = 0.0f;
static double s_yPos = 0.0f;
static int s_ChangeWindowPosition = 0;

static float s_TitleBarHeight = 64.0f;

static TextureName texture = TextureName_CStell;
static TextureName textureMinimize = TextureName_Minimize;
static TextureName textureMaximize = TextureName_Maximize;
static TextureName textureClose = TextureName_Close;

typedef int (*MouseEventFunc)();

typedef struct MouseEventStackElement
{
	MouseCode MouseButtonCode;
	MouseEventFunc Func;
} MouseEventStackElement;

static int s_LeftClickDown = 0;

static int s_CurrentIndex = 0;
static MouseEventStackElement s_MouseEventQueue[32];

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

static int MinimizeWindowUI()
{
	MinimizeWindow();
	return 1;
}

static int MaximizeWindowUI()
{
	MinMaxWindow();
	return 1;
}

static int CloseWindowUI()
{
	CloseApplication();
	return 1;
}

static int BeginDoubleClickEvent()
{
	s_StartDoubleClickEvent = 1;
	return 1;
}

static int s_LockedDelta = 0;
LSHVec2 s_MousePos = { 0.0f, 0.0f };
static int SetWindowPosition()
{
	if(s_ChangeWindowPosition)
	{
		LSHIVec2 windowPos = { 0, 0 };
		GLFWwindow* window = GetNativeWindow();
		glfwGetWindowPos(window, &windowPos.x, &windowPos.y);

		if (!s_LockedDelta)
		{
			s_MousePos.x = (float)s_xPos;
			s_MousePos.y = (float)s_yPos;
			s_LockedDelta = 1;
		}

		glfwSetWindowPos(window, (int)(windowPos.x + s_xPos - s_MousePos.x), (int)(windowPos.y + s_yPos - s_MousePos.y));
		return 1;
	}

	return 0;
}

static void HandleOnLeftClickElement(Clay_ElementId elementId, Clay_PointerData pointerInfo, intptr_t userData)
{
	s_MouseEventQueue[s_CurrentIndex].MouseButtonCode = LSH_MOUSE_BUTTON_LEFT;
	s_MouseEventQueue[s_CurrentIndex].Func = (MouseEventFunc)userData;
	s_CurrentIndex++;
}

static void HandleOnDoubleLeftClickElement(Clay_ElementId elementId, Clay_PointerData pointerInfo, intptr_t userData)
{
	if(s_StartDoubleClickEvent && s_DoubleClickDelta < s_DoubleClickThreshold)
	{
		s_MouseEventQueue[s_CurrentIndex].MouseButtonCode = LSH_MOUSE_BUTTON_LEFT;
		s_MouseEventQueue[s_CurrentIndex].Func = (MouseEventFunc)userData;
		s_CurrentIndex++;
	}
	else
	{
		s_MouseEventQueue[s_CurrentIndex].MouseButtonCode = LSH_MOUSE_BUTTON_LEFT;
		s_MouseEventQueue[s_CurrentIndex].Func = BeginDoubleClickEvent;
		s_CurrentIndex++;
	}
}

static void HandleOnLeftClickMove(Clay_ElementId elementId, Clay_PointerData pointerInfo, intptr_t userData)
{
	if (s_LeftClickDown)
		s_ChangeWindowPosition = 1;
	else
	{
		s_ChangeWindowPosition = 0;
		s_LockedDelta = 0;
	}
}

static void HandleOnMiddleClickElement(Clay_ElementId elementId, Clay_PointerData pointerInfo, intptr_t userData)
{
	s_MouseEventQueue[s_CurrentIndex].MouseButtonCode = LSH_MOUSE_BUTTON_MIDDLE;
	s_MouseEventQueue[s_CurrentIndex].Func = (MouseEventFunc)userData;
	s_CurrentIndex++;
}

static void HandleOnRightClickElement(Clay_ElementId elementId, Clay_PointerData pointerInfo, intptr_t userData)
{
	s_MouseEventQueue[s_CurrentIndex].MouseButtonCode = LSH_MOUSE_BUTTON_RIGHT;
	s_MouseEventQueue[s_CurrentIndex].Func = (MouseEventFunc)userData;
	s_CurrentIndex++;
}

static void HandleClayErrors(Clay_ErrorData errorData)
{
	LSH_ERROR("Type: %s; Msg: %s", ENUM_TO_STRING(errorData.errorType), errorData.errorText.chars);
}

static inline Clay_Dimensions MeasureText(Clay_StringSlice text, Clay_TextElementConfig* config, void* userData) {
	// Clay_TextElementConfig contains members such as fontId, fontSize, letterSpacing etc
	// Note: Clay_String->chars is not guaranteed to be null terminated
	//LSH_TRACE("MeasureText: %dx%d", text.length * config->fontSize, config->fontSize);
	return (Clay_Dimensions) {
		.width = (float)(text.length) * config->fontSize,
			.height = (float)(config->fontSize)
	};
}

static void UpdatePointerState()
{
	s_xPos = GetMouseX();
	s_yPos = GetMouseY();
	Clay_SetPointerState((Clay_Vector2) { (float)s_xPos, (float)s_yPos }, IsMouseButtonPressed(LSH_MOUSE_BUTTON_LEFT));
}

static void ListenForDoubleClick()
{
	if (s_StartDoubleClickEvent)
		s_DoubleClickDelta += s_DeltaTime;
	if (s_DoubleClickDelta >= s_DoubleClickThreshold)
	{
		s_StartDoubleClickEvent = 0;
		s_DoubleClickDelta = 0.0f;
	}
}

static void ListenForMouseButtonDown()
{
	if (IsMouseButtonPressed(LSH_MOUSE_BUTTON_LEFT))
		s_LeftClickDown = 1;
	else
		s_LeftClickDown = 0;
}

void InitUI()
{
	GLFWwindow* window = GetNativeWindow();

	WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);

	uint64_t totalMemorySize = Clay_MinMemorySize();
	Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, malloc(totalMemorySize));
	Clay_Initialize(arena, (Clay_Dimensions) { (float)data->Width, (float)data->Height }, (Clay_ErrorHandler) { HandleClayErrors });
	Clay_SetMeasureTextFunction(MeasureText, 0);

	Clay_SetLayoutDimensions((Clay_Dimensions) { (float)data->Width, (float)data->Height });

	double mouseX, mouseY;
	int isMouseDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	Clay_SetPointerState((Clay_Vector2) { (float)mouseX, (float)mouseY }, isMouseDown);

	Clay_UpdateScrollContainers(true, (Clay_Vector2) { 0.0f, 0.0f }, s_DeltaTime);

	LSH_TRACE("UI initialized");
}

void OnUpdateUI(float deltaTime)
{
	// Reset mouse event queue;
	s_CurrentIndex = 0;

	s_DeltaTime = deltaTime;

	ListenForDoubleClick();
	ListenForMouseButtonDown();
	SetWindowPosition();

	Clay_BeginLayout();
	BuildUI();
	Clay_RenderCommandArray commands = Clay_EndLayout();

	ProcessRenderUICommands(commands);

	Clay_SetLayoutDimensions((Clay_Dimensions) { (float)(GetWindowData()->Width), (float)(GetWindowData()->Height) });

	UpdatePointerState();
}

void OnEventUI(Event* event)
{
	DispatchEvent(EventTypeWindowResize, event, OnResizeWindowUI);
	//DispatchEvent(EventTypeMouseMoved, event, OnMouseMoveUI);         // Updating in UpdatePointerState();
	DispatchEvent(EventTypeMouseScrolled, event, OnMouseScrollUI);
	DispatchEvent(EventTypeKeyPressed, event, OnKeyPressUI);
	DispatchEvent(EventTypeMouseButtonPressed, event, OnMouseClickedUI);
}

void BuildUI()
{
	CLAY({
		.id = CLAY_ID("LostSheepBase"),
		.backgroundColor = (Clay_Color){0.12f, 0.12f, 0.12f, 1.0f},
		.cornerRadius = CLAY_CORNER_RADIUS(1.0f),
		.border.width = CLAY_BORDER_ALL(1),
		.border.color = (Clay_Color){0.3f, 0.3f, 0.3f, 1.0f},
		.layout = {
			.layoutDirection = CLAY_TOP_TO_BOTTOM,
			.sizing = {CLAY_SIZING_GROW(1.0f), CLAY_SIZING_GROW(1.0f)},
			.childGap = 16
		}
		})
		{
		CLAY({
			.id = CLAY_ID("TitleBar"),
			.backgroundColor = (Clay_Color){0.2f, 0.2f, 0.2f, 1.0f},
			.layout = {
				.layoutDirection = CLAY_LEFT_TO_RIGHT,
				.sizing = {CLAY_SIZING_GROW(1.0f), CLAY_SIZING_FIXED(s_TitleBarHeight)},
			}
			})
			{
			CLAY({
				.id = CLAY_ID("LostSheepIcon"),
				.image = {
					.imageData = &texture
				},
				.layout = {
					.sizing = {CLAY_SIZING_FIXED(s_TitleBarHeight), CLAY_SIZING_FIXED(s_TitleBarHeight)},
				}
				})
				{
				CLAY({
					.id = CLAY_ID("IconOverlay"),
					.backgroundColor = (Clay_Color){1.0f, 1.0f, 1.0f, Clay_Hovered() ? 0.15f : 0.0f},
					.layout = {
						.sizing = {CLAY_SIZING_GROW(1.0f), CLAY_SIZING_GROW(1.0f)},
					}
					})
				{
				}
			}
			CLAY({
				.id = CLAY_ID("TitlebarContent"),
				.backgroundColor = (Clay_Color){0.2f, 0.2f, 0.2f, 0.0f},
				.layout = {
					.layoutDirection = CLAY_TOP_TO_BOTTOM,
					.sizing = {CLAY_SIZING_GROW(1.0f), CLAY_SIZING_FIXED(s_TitleBarHeight)},
					.childAlignment = {
						.x = CLAY_ALIGN_X_CENTER,
						.y = CLAY_ALIGN_Y_TOP
					}
				}
				})
			{
				CLAY({
					.id = CLAY_ID("MenuBar"),
					.backgroundColor = (Clay_Color){0.2f, 0.2f, 0.2f, 0.0f},
					.layout = {
						.layoutDirection = CLAY_LEFT_TO_RIGHT,
						.sizing = {CLAY_SIZING_GROW(1.0f), CLAY_SIZING_FIXED(s_TitleBarHeight * 0.5f)},
						.childAlignment = {
							.x = CLAY_ALIGN_X_LEFT,
							.y = CLAY_ALIGN_Y_CENTER
						}
					}
					})
				{
					CLAY({
						.id = CLAY_ID("FileButton"),
						.backgroundColor = (Clay_Color){0.35f, 0.35f, 0.35f, Clay_Hovered() ? 1.0f : 0.0f},
						.layout = {
							.sizing = {CLAY_SIZING_FIXED(48.0f), CLAY_SIZING_GROW(1.0f)},
							.childAlignment = {
								.x = CLAY_ALIGN_X_CENTER,
								.y = CLAY_ALIGN_Y_CENTER
							}
						}
						})
					{
						CLAY_TEXT(CLAY_STRING("File"),
						CLAY_TEXT_CONFIG({
							.fontSize = 16,
							.textColor = {1.0f, 1.0f, 1.0f, 1.0f},
							.textAlignment = CLAY_TEXT_ALIGN_CENTER
							})
						);
					}
					CLAY({
						.id = CLAY_ID("WindowControlArea"),
						.layout = {
							.sizing = {CLAY_SIZING_GROW(1.0f), CLAY_SIZING_GROW(1.0f)},
						}
						})
					{
						Clay_OnHover(HandleOnDoubleLeftClickElement, (intptr_t)MaximizeWindowUI);
						CLAY({
							.id = CLAY_ID("WindowControlArea2"),
							.layout = {
								.sizing = {CLAY_SIZING_GROW(1.0f), CLAY_SIZING_GROW(1.0f)},
							}
							})
						{
							Clay_OnHover(HandleOnLeftClickMove, (intptr_t)"Move window");
						}
					}
				}
				CLAY({
					.id = CLAY_ID("TabBar"),
					.backgroundColor = (Clay_Color){0.2f, 0.2f, 0.2f, 0.0f},
					.layout = {
						.layoutDirection = CLAY_TOP_TO_BOTTOM,
						.sizing = {CLAY_SIZING_GROW(1.0f), CLAY_SIZING_GROW(1.0f)},
						.childAlignment = {
							.x = CLAY_ALIGN_X_CENTER,
							.y = CLAY_ALIGN_Y_TOP
						}
					}
					})
				{
				}
			}
			CLAY({
				.id = CLAY_ID("WindowControl"),
				.backgroundColor = (Clay_Color){0.2f, 0.2f, 0.2f, 0.0f},
				.layout = {
					.layoutDirection = CLAY_LEFT_TO_RIGHT,
					.sizing = {CLAY_SIZING_FIT(1.0f), CLAY_SIZING_FIXED(s_TitleBarHeight * 0.5f)},
					.childAlignment = {
						.x = CLAY_ALIGN_X_RIGHT,
						.y = CLAY_ALIGN_Y_CENTER
					}
				}
				})
			{
				CLAY({
					.id = CLAY_ID("MinimizeButton"),
					.backgroundColor = (Clay_Color){0.16f, 0.52f, 0.66f, Clay_Hovered() ? 1.0f : 0.0f},
					.layout = {
						.layoutDirection = CLAY_TOP_TO_BOTTOM,
						.sizing = {CLAY_SIZING_FIXED(s_TitleBarHeight * 0.7f), CLAY_SIZING_FIXED(s_TitleBarHeight * 0.5f)},
						.childAlignment = {
							.x = CLAY_ALIGN_X_CENTER,
							.y = CLAY_ALIGN_Y_CENTER
						}
					}
					})
				{
					CLAY({
						.id = CLAY_ID("MinimizeIcon"),
						.image = {
							.imageData = &textureMinimize
						},
						.layout = {
							.sizing = {CLAY_SIZING_GROW(1.0f), CLAY_SIZING_GROW(1.0f)},
						}
						})
					{
					}
					Clay_OnHover(HandleOnLeftClickElement, (intptr_t)MinimizeWindowUI);
				}

				CLAY({
					.id = CLAY_ID("MinMaxButton"),
					.backgroundColor = (Clay_Color){0.16f, 0.52f, 0.66f, Clay_Hovered() ? 1.0f : 0.0f},
					.layout = {
						.layoutDirection = CLAY_TOP_TO_BOTTOM,
						.sizing = {CLAY_SIZING_FIXED(s_TitleBarHeight * 0.7f), CLAY_SIZING_FIXED(s_TitleBarHeight * 0.5f)},
						.childAlignment = {
							.x = CLAY_ALIGN_X_CENTER,
							.y = CLAY_ALIGN_Y_CENTER
						}
					}
					})
				{
					CLAY({
						.id = CLAY_ID("MaximizeIcon"),
						.image = {
							.imageData = &textureMaximize
						},
						.layout = {
							.sizing = {CLAY_SIZING_GROW(1.0f), CLAY_SIZING_GROW(1.0f)},
						}
						})
					{
					}

					Clay_OnHover(HandleOnLeftClickElement, (intptr_t)MaximizeWindowUI);
				}

				CLAY({
					.id = CLAY_ID("CloseButton"),
					.backgroundColor = (Clay_Color){0.90f, 0.29f, 0.24f, Clay_Hovered() ? 1.0f : 0.0f},
					.layout = {
						.layoutDirection = CLAY_TOP_TO_BOTTOM,
						.sizing = {CLAY_SIZING_FIXED(s_TitleBarHeight * 0.7f), CLAY_SIZING_FIXED(s_TitleBarHeight * 0.5f)},
						.childAlignment = {
							.x = CLAY_ALIGN_X_CENTER,
							.y = CLAY_ALIGN_Y_CENTER
						}
					}
					})
				{
					CLAY({
						.id = CLAY_ID("CloseIcon"),
						.image = {
							.imageData = &textureClose
						},
						.layout = {
							.sizing = {CLAY_SIZING_GROW(1.0f), CLAY_SIZING_GROW(1.0f)},
						}
						})
					{
					}
					Clay_OnHover(HandleOnLeftClickElement, (intptr_t)CloseWindowUI);
				}
			}
		}
	}
	//CLAY({
	//	.id = CLAY_ID("OutestContainer"),
	//	.backgroundColor = (Clay_Color){0.25f, 0.25f, 0.25f, 1.0f},
	//	.layout = {
	//		.layoutDirection = CLAY_LEFT_TO_RIGHT,
	//		.sizing = {CLAY_SIZING_GROW(1.0f), CLAY_SIZING_GROW(1.0f)},
	//		.padding = CLAY_PADDING_ALL(16),
	//		.childGap = 16
	//	}
	//	})
	//{
	//	CLAY({
	//		.id = CLAY_ID("OuterContainer0"),
	//		.backgroundColor = (Clay_Color){0.15f, 0.15f, 0.15f, 1.0f},
	//		.layout = {
	//			.layoutDirection = CLAY_TOP_TO_BOTTOM,
	//			.sizing = {CLAY_SIZING_PERCENT(Clay_Hovered() ? 0.75f : 0.5f), CLAY_SIZING_GROW(1.0f)},
	//			.padding = CLAY_PADDING_ALL(16),
	//			.childGap = 16
	//		}
	//		})
	//	{
	//		Clay_OnHover(HandleOnClickElement, (intptr_t)"Lost Sheep");

	//		CLAY({
	//			.id = CLAY_ID("Header01a") ,
	//			.image = {
	//				.imageData = &texture
	//			},
	//			.cornerRadius = CLAY_CORNER_RADIUS(8.0f),
	//			.border.width = CLAY_BORDER_ALL(Clay_Hovered() ? 2 : 0),
	//			.border.color = (Clay_Color){0.8f, 0.8f, 0.8f, 1.0f},
	//			.layout = {
	//				.sizing = {CLAY_SIZING_GROW(1.0f), CLAY_SIZING_GROW(1.0f)},
	//				.padding = CLAY_PADDING_ALL(16)
	//				}
	//			})
	//		{
	//			Clay_OnHover(HandleOnClickElement, (intptr_t)"Lost Sheep");
	//		}

	//	}
	//	CLAY({
	//		.id = CLAY_ID("OuterContainer"),
	//		.layout = {
	//			.layoutDirection = CLAY_TOP_TO_BOTTOM,
	//			.sizing = {CLAY_SIZING_GROW(1.0f), CLAY_SIZING_GROW(1.0f)},
	//			.padding = CLAY_PADDING_ALL(16),
	//			.childGap = 16
	//		}
	//		})
	//	{
	//		CLAY({
	//			.id = CLAY_ID("Header") ,
	//			.backgroundColor = (Clay_Color) { 0.92f, 0.51f, !Clay_Hovered() ? 0.62f : 0.8f, 1.0f },
	//			.cornerRadius = CLAY_CORNER_RADIUS(8.0f),
	//			.border.width = CLAY_BORDER_ALL(Clay_Hovered() ? 2 : 0),
	//			.border.color = (Clay_Color){0.8f, 0.8f, 0.8f, 1.0f},
	//			.layout = {
	//				.sizing = {CLAY_SIZING_GROW(1.0f), CLAY_SIZING_PERCENT(0.15f)},
	//				.padding = CLAY_PADDING_ALL(16),
	//				.childAlignment = {
	//				//.x = CLAY_ALIGN_X_CENTER,
	//				.y = CLAY_ALIGN_Y_CENTER
	//				}
	//			}
	//			})
	//		{
	//			CLAY_TEXT(CLAY_STRING("Lost Sheep"),
	//				CLAY_TEXT_CONFIG({
	//				.fontSize = 48,
	//				.textColor = {0.0f, 1.0f, 1.0f, 1.0f},
	//				.textAlignment = CLAY_TEXT_ALIGN_CENTER
	//				}));
	//		}
	//		for(int i = 0; i < 3; i++)
	//		{
	//			CLAY({
	//				.id = CLAY_IDI("Header2", i) ,
	//				.image = {
	//					.imageData = &texture
	//				},
	//				.cornerRadius = CLAY_CORNER_RADIUS(8.0f),
	//				.border.width = CLAY_BORDER_ALL(Clay_Hovered() ? 2 : 0),
	//				.border.color = (Clay_Color){0.8f, 0.8f, 0.8f, 1.0f},
	//				.layout = {
	//					.sizing = {CLAY_SIZING_GROW(1), CLAY_SIZING_GROW(1)},
	//					.padding = CLAY_PADDING_ALL(16),
	//					.childAlignment = {
	//						.x = CLAY_ALIGN_X_CENTER,
	//						.y = CLAY_ALIGN_Y_CENTER
	//					}
	//				}
	//				})
	//			{
	//				CLAY_TEXT(CLAY_STRING("Lost Sheep; My memo for daily activities"),
	//					CLAY_TEXT_CONFIG({
	//						.fontSize = Clay_Hovered() ? 24 : 18,
	//						.textColor = {1.0f, 1.0f, 1.0f, 1.0f},
	//						.textAlignment = CLAY_TEXT_ALIGN_CENTER
	//					}));
	//				Clay_OnHover(HandleOnClickElement, (intptr_t)"Lost Sheep");
	//			}
	//		}
	//	}
	// }
	//
	//bool isHovered = Clay_PointerOver(Clay_GetElementId(CLAY_STRING("Header")));
	//if (isHovered && IsMouseButtonPressed(LSH_MOUSE_BUTTON_MIDDLE))
	//{
	//	LSH_WARN("Handling input example");
	//}
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
	if (*((int*)event->Data) == LSH_KEY_R)
	{
		//RecompileShader("Rectangle.glsl");
		return 1;
	}
	if (*((int*)event->Data) == LSH_KEY_D)
	{
		if (!s_DebugLayout)
			s_DebugLayout = 1;
		else
			s_DebugLayout = 0;

		Clay_SetDebugModeEnabled(s_DebugLayout);
		return 1;
	}
	return 0;
}

int OnMouseClickedUI(Event* event)
{
	if (*((int*)event->Data) == s_MouseEventQueue[s_CurrentIndex - 1].MouseButtonCode)
	{
		if (s_MouseEventQueue[s_CurrentIndex - 1].Func)
		{
			s_MouseEventQueue[s_CurrentIndex - 1].Func();
			s_CurrentIndex--;
			return 1;
		}
	}

	return 0;
}

void ShutdownUI()
{
}
