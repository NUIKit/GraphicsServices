//
//  GSEvent.c
//  GraphicsServices
//
//  Created by Robert Widmann on 7/31/15.
//  Copyright © 2015 CodaFi. All rights reserved.
//

#include "GSEvent.h"
#include "GSCoreFoundationBridge.h"
#include "GSPrivate.h"

CFTypeRef _GSEventCopy(CFAllocatorRef, CFTypeRef);
CFStringRef _GSEventCopyDescription(CFTypeRef);
Boolean _GSEventEqualToEvent(CFTypeRef, CFTypeRef);

static CFTypeID kGSEventTypeID = _kCFRuntimeNotATypeID;

static const CFRuntimeClass _GSEventClass = {
	0,
	"GSEventClass",
	NULL,       // init
	_GSEventCopy, // copy
	NULL,		// dealloc
	_GSEventEqualToEvent, // isEqual
	NULL,		// hash
	NULL,       // formatting description
	_GSEventCopyDescription, // description
};


struct __GSEvent {
	CFRuntimeBase _base; // 0x0
	GSEventType type; // 0x10
	//
	//
	//
	CGFloat windowLocationX; // 0x28
	CGFloat windowLocationY; // 0x30
	//
	CFAbsoluteTime timestamp; // 0x40
};

void _GSEventClassInitialize() {
	kGSEventTypeID = _CFRuntimeRegisterClass(&_GSEventClass);
}

CFTypeRef _GSEventCopy(CFAllocatorRef allocator, CFTypeRef cf) {
	GSEventRef copiedEvent = (GSEventRef)_CFRuntimeCreateInstance(NULL, kGSEventTypeID, /*cf->usagePage*/ 0, 0);
	if (copiedEvent != NULL) {
		memcpy(copiedEvent + sizeof(CFRuntimeBase), cf + sizeof(CFRuntimeBase), sizeof(struct __GSEvent) - sizeof(CFRuntimeBase));
	}
	return copiedEvent;
}

CFStringRef _GSEventCopyDescription(CFTypeRef t) {
	struct __GSEvent *evt = (struct __GSEvent *)t;
	return CFStringCreateWithFormat(CFGetAllocator(t), NULL, CFSTR("<GSEvent %p>{type = %lx, windowLoc = (%f, %f)}"), t, GSEventGetTypeID(), evt->windowLocationX, evt->windowLocationY);
}

Boolean _GSEventEqualToEvent(CFTypeRef cf1, CFTypeRef cf2) {
	if (cf1 == NULL || cf2 == NULL) {
		return false;
	}
	
	struct __GSEvent *evt1 = (struct __GSEvent *)cf1;
	struct __GSEvent *evt2 = (struct __GSEvent *)cf2;
	return (evt1->type == evt2->type) && (evt1->timestamp == evt2->timestamp);
}

CFTypeID GSEventGetTypeID() {
	return kGSEventTypeID;
}

GSEventRef GSEventCreateWithCGEvent(CFAllocatorRef allocator, CGEventRef event) {
	uint32_t size = sizeof(struct __GSEvent) - sizeof(CFRuntimeBase);
	GSEventRef memory = (void *)_CFRuntimeCreateInstance(allocator, GSEventGetTypeID(), size, NULL);
	if (NULL == memory) {
		return NULL;
	}
	
	return memory;
}

GSEventType GSEventGetType(GSEventRef ref) {
	if (ref == NULL) {
		return -1;
	}
	return ref->type;
}

CGPoint GSEventGetLocationInWindow(GSEventRef ref) {
	if (ref == NULL) {
		return CGPointZero;
	}
	return (CGPoint){ ref->windowLocationX, ref->windowLocationY };
}

CFAbsoluteTime GSEventGetTimestamp(GSEventRef ref) {
	if (ref == NULL) {
		return 0;
	}
	return ref->timestamp;
}

void GSEventRun() {
	GSEventRunModal(false);
}

void GSEventRegisterEventCallBack(void (^callback)(GSEventRef)) {
	___eventCallBack = callback;
}

static CFMutableArrayRef ___runLoopModeStack;
static bool ___exitRunModal;

void GSEventPushRunLoopMode(CFStringRef mode) {
	if (___runLoopModeStack == NULL) {
		___runLoopModeStack = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
	}
	CFArrayAppendValue(___runLoopModeStack, mode);
	CFRunLoopStop(___eventRunLoop);
}

void GSEventPopRunLoopMode(CFStringRef mode) {
	if ((___runLoopModeStack == NULL) || (CFArrayGetCount(___runLoopModeStack) <= 0)) {
		CFRunLoopStop(___eventRunLoop);
		return;
	}

	CFStringRef topLoopMode = CFArrayGetValueAtIndex(___runLoopModeStack, CFArrayGetCount(___runLoopModeStack) - 1);
	if ((topLoopMode != NULL) && (CFStringCompare(mode, topLoopMode, 0) == kCFCompareEqualTo)) {
		CFArrayRemoveValueAtIndex(___runLoopModeStack, CFArrayGetCount(___runLoopModeStack) - 1);
	}
	CFRunLoopStop(___eventRunLoop);
}

void GSEventRunModal(bool disallowsRestarts) {
	while (true) {
		if (disallowsRestarts == false) {
			if ((___runLoopModeStack == NULL) || (CFArrayGetCount(___runLoopModeStack) <= 0)) {
				fprintf(stderr, "%s: NULL run loop mode. Exiting loop\n", "GSEventRunModal");
				___exitRunModal = false;
				return;
			}
		}
		
		if (___exitRunModal) {
			___exitRunModal = false;
			return;
		}
		
		CFStringRef topRunLoopMode = CFArrayGetValueAtIndex(___runLoopModeStack, CFArrayGetCount(___runLoopModeStack) - 1);
		if (topRunLoopMode == NULL) {
			fprintf(stderr, "%s: NULL run loop mode. Exiting loop\n", "GSEventRunModal");
			___exitRunModal = false;
			return;
		}
		
		CFRunLoopRunInMode(topRunLoopMode, kCFAbsoluteTimeIntervalSince1970, disallowsRestarts);
	}
}

void GSEventStopModal() {
	___exitRunModal = true;
	CFRunLoopStop(CFRunLoopGetCurrent());
}

static mach_port_name_t ___applicationPort;


void __GSEventInitializeApp(dispatch_queue_t queue) {
	static bool _initialized;
	
	if (_initialized == true) {
		return;
	}
	_initialized = true;
	
	__GSEventInitializeShared(queue);
	___applicationPort = CFMachPortGetPort(CGWindowServerCreateServerPort());
	
	if (queue != NULL) {
		mach_port_limits_t qlimits;
		qlimits.mpl_qlimit = 0x100;
		mach_port_set_attributes(mach_task_self(), ___applicationPort, MACH_PORT_LIMITS_INFO, (mach_port_info_t)&qlimits, MACH_PORT_LIMITS_INFO_COUNT);
		_GSAddSourceForEventPort(___applicationPort, queue);
	}
	
	if (___eventRunLoop != NULL) {
		_GSAddRunLoopSourceForEventPort(___applicationPort, kCFRunLoopCommonModes);
		_GSAddRunLoopSourceForEventPort(___applicationPort, kGSEventReceiveRunLoopMode);
	}
//	int err = mach_port_insert_right(mach_task_self(), ___applicationPort, ___applicationPort, MACH_MSG_TYPE_MAKE_SEND);
//	if (err != KERN_SUCCESS) {
//		fprintf(stderr, "mach_port_insert_right() error: %s\n", mach_error_string(err));
//		abort();
//	}
}

CF_EXPORT void GSEventInitialize() {
	__GSEventInitializeApp(NULL);
}

