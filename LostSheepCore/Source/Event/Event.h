#pragma once

#include <stdint.h>

#define EVENT_TYPE_STRING(eventType) #eventType

typedef void (*EventCallbackHandlefn)(void* Event);

typedef int (*EventCallbackfn)(void* Event);

typedef enum EventType
{
	None = 0,
	EventTypeWindowClose, EventTypeWindowResize, EventTypeWindowMove,
	EventTypeKeyPressed, EventTypeKeyReleased, EventTypeKeyRepeat, EventTypeKeyTyped,
	EventTypeMouseButtonPressed, EventTypeMouseButtonReleased, EventTypeMouseButtonRepeat, EventTypeMouseMoved, EventTypeMouseScrolled
} EventType;

typedef struct Event
{
	EventType Type;
	void* Data;
	uint32_t Size;

	int Handled;
} Event;

int DispatchEvent(EventType type, Event* event, EventCallbackfn callback);