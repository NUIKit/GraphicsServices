//
//  GSApp.h
//  GraphicsServices
//
//  Created by Robert Widmann on 8/1/15.
//  Copyright © 2015 CodaFi. All rights reserved.
//

#ifndef GSAPP_H
#define GSAPP_H

#include <CoreFoundation/CoreFoundation.h>
#include <CoreGraphics/CoreGraphics.h>
#include <Availability.h>
#include <GraphicsServices/GSEvent.h>


#pragma mark - Application Initialization


/// Initializes the event pump.
CF_EXPORT void GSAppInitialize(void);


#pragma mark - Run Loops


/// The run loop mode GraphicsServices spins the event loop in.
CF_EXPORT const CFStringRef kGSEventReceiveRunLoopMode;

/// Spins the runloop in the current mode.
CF_EXPORT void GSAppRun(void);

/// Pushes a new run loop mode onto the top of the stack.
CF_EXPORT void GSAppPushRunLoopMode(CFStringRef mode);

/// Pops the topmost run loop mode from the stack.
///
/// If there are no modes or the last mode has been popped the application will terminate.
CF_EXPORT void GSAppPopRunLoopMode(CFStringRef mode);

/// Spins the runloop in the topmost mode by initiating a modal session in the previous runloop.
///
/// If the `disallowsRestarts` flag is set the runloop will not automatically restart once it has
/// stopped.
CF_EXPORT void GSAppRunModal(bool disallowsRestarts);

/// Stops the topmost runloop and transfers control to the initiator of the modal session.
CF_EXPORT void GSAppStopModal(void);


#pragma mark - Callbacks


/// Registers a callback with the event pump.
CF_EXPORT void GSAppRegisterEventCallBack(void (^callback)(GSEventRef));



#endif /* GSAPP_H */
