#include "Event.h"

int DispatchEvent(EventType type, Event* event, EventCallbackfn callback)
{
	if (!event->Handled)
	{
		if (event->Type == type)
		{
			if(callback(event))
			{
				event->Handled = 1;
				return 1;
			}
		}
	}

	return 0;
}
