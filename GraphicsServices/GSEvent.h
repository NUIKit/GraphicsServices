//
//  GSEvent.h
//  GraphicsServices
//
//  Created by Robert Widmann on 7/31/15.
//  Copyright © 2015 CodaFi. All rights reserved.
//

#ifndef GSEVENT_H
#define GSEVENT_H

#include <mach/message.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreGraphics/CoreGraphics.h>
#include <Availability.h>

typedef struct __GSEvent *GSEventRef;

typedef enum __GSEventType {
	kGSEventTypePlaceholder = -1,
} GSEventType;

/// Returns the type identifier for the GSEvent opaque type.
CF_EXPORT CFTypeID GSEventGetTypeID();

///
CF_EXPORT GSEventRef GSEventCreateWithCGEvent(CFAllocatorRef allocator, CGEventRef event);

/// Returns the type of the given event.
CF_EXPORT GSEventType GSEventGetType(GSEventRef);

/// Returns the location of the given event in the base coordinate system of the associated window.
CF_EXPORT CGPoint GSEventGetLocationInWindow(GSEventRef);

/// The time when the event occurred in seconds since system startup.
CF_EXPORT CFAbsoluteTime GSEventGetTimestamp(GSEventRef);


#pragma mark - Application Initialization

/// Initializes the event pump.
CF_EXPORT void GSEventInitialize();


#pragma mark - Run Loops


/// The run loop mode GraphicsServices spins the event loop in.
CF_EXPORT const CFStringRef kGSEventReceiveRunLoopMode;

/// Spins the runloop in the current mode.
CF_EXPORT void GSEventRun();

/// Pushes a new run loop mode onto the top of the stack.
CF_EXPORT void GSEventPushRunLoopMode(CFStringRef mode);

/// Pops the topmost run loop mode from the stack.
///
/// If there are no modes or the last mode has been popped the application will terminate.
CF_EXPORT void GSEventPopRunLoopMode(CFStringRef mode);

/// Spins the runloop in the topmost mode by initiating a modal session in the previous runloop.
///
/// If the `disallowsRestarts` flag is set the runloop will not automatically restart once it has
/// stopped.
CF_EXPORT void GSEventRunModal(bool disallowsRestarts);

/// Stops the topmost runloop and transfers control to the initiator of the modal session.
CF_EXPORT void GSEventStopModal();


#pragma mark - Callbacks

/// Registers a callback with the event pump.
CF_EXPORT void GSEventRegisterEventCallBack(void (^callback)(GSEventRef));


#endif
