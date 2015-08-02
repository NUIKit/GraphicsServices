//
//  GSPrivate.c
//  GraphicsServices
//
//  Created by Robert Widmann on 7/31/15.
//  Copyright © 2015 CodaFi. All rights reserved.
//

#include "GSPrivate.h"
#include "GSApp.h"
#include "GSEvent.h"
#include "GSEventQueue.h"

mach_port_name_t ___lastRecievedPort;
static CFRunLoopSourceRef ___signalRunLoopSource;
CFRunLoopRef ___eventRunLoop;
static dispatch_queue_t ___eventDeliveryQueue;
void (^___eventCallBack)(GSEventRef);

static void _ReceiveEvent(CFMachPortRef port, void *msg, CFIndex size, void *info);
static void _PurpleEventSignalCallback(void *context);
static void __PurpleEventCallback(GSEventRef event);

void _GSAddRunLoopSourceForEventPort(mach_port_t p, CFStringRef mode, int64_t contextID) {
	CFMachPortContext context = (CFMachPortContext){
		1,
		(void *)contextID,
		NULL,
		NULL,
		NULL,
	};
	CFMachPortRef port = CFMachPortCreateWithPort(NULL, p, _ReceiveEvent, &context, NULL);
	CFRunLoopSourceRef eventSource = CFMachPortCreateRunLoopSource(NULL, port, -1);
	CFRunLoopAddSource(___eventRunLoop, eventSource, mode);
	CFRelease(eventSource);
}

void _GSAddSourceForEventPort(mach_port_t eport, dispatch_queue_t queue, int64_t contextID) {
	if (queue != NULL) {
		dispatch_source_t source = dispatch_source_create(DISPATCH_SOURCE_TYPE_MACH_RECV, (mach_port_name_t)eport, 0, queue);
		dispatch_source_set_event_handler(source, ^{
			mach_port_name_t port = (mach_port_name_t)dispatch_source_get_handle(source);
			if (port != 0) {
				___lastRecievedPort = port;
			}
			_ReceiveEvent(NULL, NULL, 0, NULL);
			do {
//				__PurpleEventCallback(receivedEvent);
				if (GSEventQueueIsEmpty(GSEventQueueGetThreadLocalQueue())) {
					break;
				}
			} while (true);
			return;
		});
		dispatch_resume(source);
	} else {
		_GSAddRunLoopSourceForEventPort(eport, kCFRunLoopCommonModes, contextID);
		_GSAddRunLoopSourceForEventPort(eport, kGSEventReceiveRunLoopMode, contextID);
	}
}

static void _ReceiveEvent(CFMachPortRef port, void *msg, CFIndex size, void *info) {
	CGEventRef event = CGEventCreateNextEvent((int64_t)info);
	if (event == NULL) {
		return;
	}
	
	GSEventRef gsEvent = GSEventCreateWithCGEvent(NULL, event);
	if (gsEvent == NULL) {
		return;
	}
	__PurpleEventCallback(gsEvent);
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
		CFRunLoopSourceContext context = (CFRunLoopSourceContext){
			.perform = _PurpleEventSignalCallback,
		};
		___signalRunLoopSource = CFRunLoopSourceCreate(NULL, -1, &context);
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
//		CGEventRef recievedEvent = NULL;
//		while ((recievedEvent = CGEventCreateNextEvent()) != NULL) {
//			GSEventRef evt = GSEventCreateWithCGEvent(NULL, recievedEvent);
//			if (evt != NULL) {
//				GSEventQueuePushEvent(evt);
//			}
//		}
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



