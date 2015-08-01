//
//  GSPrivate.c
//  GraphicsServices
//
//  Created by Robert Widmann on 7/31/15.
//  Copyright © 2015 CodaFi. All rights reserved.
//

#include "GSEvent.h"
#include "GSPrivate.h"
#include "GSEventQueue.h"

mach_port_name_t ___lastRecievedPort;
static CFRunLoopSourceRef ___signalRunLoopSource;
CFRunLoopRef ___eventRunLoop;
static dispatch_queue_t ___eventDeliveryQueue;
void (^___eventCallBack)(GSEventRef);

static GSEventRef _ReceiveEvent(mach_port_name_t port);
static void _PurpleEventSignalCallback(void *context);
static void __PurpleEventCallback(GSEventRef event);


void _GSAddRunLoopSourceForEventPort(mach_port_name_t port, CFStringRef mode) {
	CFRunLoopSourceContext context = (CFRunLoopSourceContext){
		.perform = _PurpleEventSignalCallback,
	};
	CFRunLoopSourceRef eventSource = CFRunLoopSourceCreate(NULL, -1, &context);
	CFRunLoopAddSource(___eventRunLoop, eventSource, mode);
	CFRelease(eventSource);
}

void _GSAddSourceForEventPort(mach_port_name_t port, dispatch_queue_t queue) {
	if (queue != NULL) {
		dispatch_source_t source = dispatch_source_create(DISPATCH_SOURCE_TYPE_MACH_RECV, (mach_port_name_t)port, 0, queue);
		dispatch_source_set_event_handler(source, ^{
			mach_port_name_t port = (mach_port_name_t)dispatch_source_get_handle(source);
			if (port != 0) {
				___lastRecievedPort = port;
			}
			GSEventRef receivedEvent = _ReceiveEvent(port);
			do {
				__PurpleEventCallback(receivedEvent);
				if (GSEventQueueIsEmpty(GSEventQueueGetThreadLocalQueue())) {
					break;
				}
				receivedEvent = NULL;
			} while (true);
			return;
		});
		dispatch_resume(source);
	} else {
		_GSAddRunLoopSourceForEventPort(port, kCFRunLoopCommonModes);
		_GSAddRunLoopSourceForEventPort(port, kGSEventReceiveRunLoopMode);
	}
}

static GSEventRef _ReceiveEvent(mach_port_name_t port) {
	CGEventRef event = CGEventCreateNextEvent();
	if (event == NULL) {
		return NULL;
	}

	return GSEventCreateWithCGEvent(NULL, event);
}

void __GSEventInitializeShared(dispatch_queue_t callbackQueue) {
	if (callbackQueue != NULL) {
		___eventDeliveryQueue = callbackQueue;
		dispatch_retain(___eventDeliveryQueue);
	} else {
		___eventRunLoop = CFRunLoopGetCurrent();
		CFRetain(___eventRunLoop);
	}
	if (___eventRunLoop != NULL) {
		___signalRunLoopSource = CFRunLoopSourceCreate(NULL, -1, NULL);
		CFRunLoopAddSource(___eventRunLoop, ___signalRunLoopSource, kCFRunLoopCommonModes);
		CFRunLoopAddSource(___eventRunLoop, ___signalRunLoopSource, kGSEventReceiveRunLoopMode);
	}
}

static void _PurpleEventSignalCallback(void *context) {
	__PurpleEventCallback(NULL);
}

static void __PurpleEventCallback(GSEventRef event) {
	if (event != NULL) {
		GSEventQueuePushEvent(event);
	}
	if (___lastRecievedPort != 0) {
		GSEventRef recievedEvent = NULL;
		while ((recievedEvent = _ReceiveEvent(___lastRecievedPort)) != NULL) {
			GSEventQueuePushEvent(recievedEvent);
		}
	}

	event = GSEventQueueGetCurrentEvent();
	if (event == NULL) {
		goto fail;
	}
	
	if (___eventCallBack != NULL) {
		___eventCallBack(event);
	}
	
	CFRelease(event);
	
fail:
	GSEventQueuePopEvent();
	if (___signalRunLoopSource != NULL) {
		CFRunLoopSourceSignal(___signalRunLoopSource);
	}
}



