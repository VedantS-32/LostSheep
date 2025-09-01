#pragma once

#include <stdint.h>

#define EVENT_TYPE_STRING(eventType) #eventType

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

typedef void (*EventCallbackHandlefn)(Event* event);

typedef int (*EventCallbackfn)(Event* event);

int DispatchEvent(EventType type, Event* event, EventCallbackfn callback);