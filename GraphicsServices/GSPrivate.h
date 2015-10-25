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
void _GSEventClassInitialize(void);

CF_EXPORT
void _GSAddRunLoopSourceForEventPort(mach_port_t port, CFStringRef mode, int64_t contextID);

CF_EXPORT
void _GSAddSourceForEventPort(mach_port_t port, dispatch_queue_t queue, int64_t contextID);

CF_EXPORT
CGEventRef CGEventCreateNextEvent(int64_t);

typedef unsigned short CGSEventRecordVersion;
typedef unsigned long long CGSEventRecordTime;  /* nanosecond timer */
typedef unsigned long CGSEventFlag;
typedef unsigned long CGSByteCount;
typedef uint32_t CGSConnectionID;

typedef struct {
	NXEventData eventData;
	SInt32 _padding[4];
} CGSEventRecordData;

typedef struct _CGSEventRecord {
	CGSEventRecordVersion major; /*0x0*/
	CGSEventRecordVersion minor; /*0x2*/
	CGSByteCount length;         /*0x4*/ /* Length of complete event record */
	CGSEventType type;           /*0x8*/ /* An event type from above */
	CGPoint location;            /*0x10*/ /* Base coordinates (global), from upper-left */
	CGPoint windowLocation;      /*0x20*/ /* Coordinates relative to window */
	CGSEventRecordTime time;     /*0x30*/ /* nanoseconds since startup */
	CGSEventFlag flags;         /* key state flags */
	CGWindowID window;         /* window number of assigned window */
	CGSConnectionID connection; /* connection the event came from */
	struct __CGEventSourceData {
		int source;
		unsigned int sourceUID;
		unsigned int sourceGID;
		unsigned int flags;
		unsigned long long userData;
		unsigned int sourceState;
		unsigned short localEventSuppressionInterval;
		unsigned char suppressionIntervalFlags;
		unsigned char remoteMouseDragFlags;
		unsigned long long serviceID;
	} eventSource;
	struct _CGEventProcess {
		int pid;
		unsigned int psnHi;
		unsigned int psnLo;
		unsigned int targetID;
		unsigned int flags;
	} eventProcess;
	NXEventData eventData;
	SInt32 _padding[4];
	void *ioEventData;
	unsigned short _field16;
	unsigned short _field17;
	struct _CGSEventAppendix {
		unsigned short windowHeight;
		unsigned short mainDisplayHeight;
		unsigned short *unicodePayload;
		unsigned int eventOwner;
		unsigned char passedThrough;
	} *appendix;
	unsigned int _field18;
	bool passedThrough;
	CFDataRef data;
} CGSEventRecord;

/// Gets the event record for a given `CGEventRef`.
///
/// For Carbon events, use `GetEventPlatformEventRecord`.
CG_EXTERN CGError CGEventGetEventRecord(CGEventRef event, CGSEventRecord *outRecord, size_t recSize);

#endif /* GSPRIVATE_H */
