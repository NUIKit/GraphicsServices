//
//  GSPrivate.h
//  GraphicsServices
//
//  Created by Robert Widmann on 7/31/15.
//  Copyright © 2015 CodaFi. All rights reserved.
//

#ifndef GSPRIVATE_H
#define GSPRIVATE_H

#include <CoreFoundation/CoreFoundation.h>
#include "GSEvent.h"
#include "GSEventQueue.h"

CF_EXPORT CFRunLoopRef ___eventRunLoop;
CF_EXPORT mach_port_name_t ___lastRecievedPort;
CF_EXPORT void (^___eventCallBack)(GSEventRef);

CF_EXPORT
void __GSEventInitializeShared(dispatch_queue_t);

CF_EXPORT
void _GSEventClassInitialize();

CF_EXPORT
void _GSAddRunLoopSourceForEventPort(mach_port_name_t, CFStringRef);

CF_EXPORT
void _GSAddSourceForEventPort(mach_port_name_t, dispatch_queue_t);

CF_EXPORT
CGEventRef CGEventCreateNextEvent();

#endif /* GSPRIVATE_H */