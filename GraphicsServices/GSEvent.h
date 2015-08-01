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


#endif
