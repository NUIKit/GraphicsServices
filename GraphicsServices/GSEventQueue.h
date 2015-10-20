//
//  GSEventQueue.h
//  GraphicsServices
//
//  Created by Robert Widmann on 8/1/15.
//  Copyright © 2015 CodaFi. All rights reserved.
//

#ifndef GSEVENTQUEUE_H
#define GSEVENTQUEUE_H

#include "GSEvent.h"

typedef struct GSPurpleEventPair {
	GSEventRef event;
	struct GSPurpleEventPair *next;
} *GSPurpleEventPairRef;

typedef struct GSPurpleQueue {
	GSPurpleEventPairRef front;
	GSPurpleEventPairRef back;
} *GSPurpleQueueRef;

/// Returns the event queue local to the current thread of execution.
CF_EXPORT GSPurpleQueueRef GSEventQueueGetThreadLocalQueue(void);

/// Returns true iff the given queue is empty, else false.
CF_EXPORT bool GSEventQueueIsEmpty(GSPurpleQueueRef);

/// Pushes an event to the back of the thread's event queue.
CF_EXPORT void GSEventQueuePushEvent(GSEventRef event);

/// Pops and deallocates an event from the front of the thread's event queue.
CF_EXPORT void GSEventQueuePopEvent(void);

/// Peeks at the event at the front of the thread's event queue.
CF_EXPORT GSEventRef GSEventQueueGetCurrentEvent(void);

#endif /* GSEVENTQUEUE_H */
