//
//  GSApp.c
//  GraphicsServices
//
//  Created by Robert Widmann on 8/1/15.
//  Copyright © 2015 CodaFi. All rights reserved.
//

#include "GSApp.h"
#include "GSPrivate.h"

void GSAppRun() {
	GSAppRunModal(false);
}

void GSAppRegisterEventCallBack(void (^callback)(GSEventRef)) {
	___eventCallBack = callback;
}

static CFMutableArrayRef ___runLoopModeStack;
static bool ___exitRunModal;

void GSAppPushRunLoopMode(CFStringRef mode) {
	if (___runLoopModeStack == NULL) {
		___runLoopModeStack = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
	}
	CFArrayAppendValue(___runLoopModeStack, mode);
	CFRunLoopStop(___eventRunLoop);
}

void GSAppPopRunLoopMode(CFStringRef mode) {
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

void GSAppRunModal(bool disallowsRestarts) {
	while (true) {
		if (disallowsRestarts == false) {
			if ((___runLoopModeStack == NULL) || (CFArrayGetCount(___runLoopModeStack) <= 0)) {
				fprintf(stderr, "%s: NULL run loop mode. Exiting loop\n", "GSAppRunModal");
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
			fprintf(stderr, "%s: NULL run loop mode. Exiting loop\n", "GSAppRunModal");
			___exitRunModal = false;
			return;
		}
		
		CFRunLoopRunInMode(topRunLoopMode, kCFAbsoluteTimeIntervalSince1970, disallowsRestarts);
	}
}

void GSAppStopModal() {
	___exitRunModal = true;
	CFRunLoopStop(CFRunLoopGetCurrent());
}

static mach_port_t ___applicationPort;
extern CGError CGSGetEventPort(int64_t, mach_port_t *);
extern int64_t CGSMainConnectionID();

void __GSEventInitializeApp(dispatch_queue_t queue) {
	static bool _initialized;
	
	if (_initialized == true) {
		return;
	}
	_initialized = true;
	
	__GSEventInitializeShared(queue);
	
	int64_t context = CGSMainConnectionID();
	if (CGSGetEventPort(context, &___applicationPort) != kCGErrorSuccess) {
		fprintf(stderr, "Fatal: failed to create main application connection port");
		abort();
	}
	
	if (queue != NULL) {
		mach_port_limits_t qlimits;
		qlimits.mpl_qlimit = 0x100;
		mach_port_set_attributes(mach_task_self(), ___applicationPort, MACH_PORT_LIMITS_INFO, (mach_port_info_t)&qlimits, MACH_PORT_LIMITS_INFO_COUNT);
		_GSAddSourceForEventPort(___applicationPort, queue, context);
	}
	
	if (___eventRunLoop != NULL) {
		_GSAddRunLoopSourceForEventPort(___applicationPort, kCFRunLoopCommonModes, context);
		_GSAddRunLoopSourceForEventPort(___applicationPort, kGSEventReceiveRunLoopMode, context);
	}
//	int err = mach_port_insert_right(mach_task_self(), ___applicationPort, ___applicationPort, MACH_MSG_TYPE_MAKE_SEND);
//	if (err != KERN_SUCCESS) {
//		fprintf(stderr, "mach_port_insert_right() error: %s\n", mach_error_string(err));
//		abort();
//	}
}

void GSAppInitialize() {
	__GSEventInitializeApp(NULL);
}
