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
typedef int CGSConnectionID;

typedef enum {
	kCGSEventNotificationNullEvent = 0,

	kCGSEventNotificationLeftMouseDown,
	kCGSEventNotificationLeftMouseUp,
	kCGSEventNotificationRightMouseDown,
	kCGSEventNotificationRightMouseUp,
	kCGSEventNotificationMouseMoved,
	kCGSEventNotificationLeftMouseDragged,
	kCGSEventNotificationRightMouseDragged,

	kCGSEventNotificationMouseEntered,
	kCGSEventNotificationMouseExited,

	kCGSEventNotificationKeyDown,
	kCGSEventNotificationKeyUp,

	kCGSEventNotificationFlagsChanged,

	kCGSEventNotificationKitDefined,
	kCGSEventNotificationSystemDefined,
	kCGSEventNotificationApplicationDefined,

	kCGSEventNotificationTimer,

	kCGSEventNotificationCursorUpdate,

	kCGSEventNotificationSuspend,
	kCGSEventNotificationResume,

	kCGSEventNotificationNotification,

	kCGSEventNotificationScrollWheel,

	kCGSEventNotificationTabletPointer,
	kCGSEventNotificationTabletProximity,

	kCGSEventNotificationOtherMouseDown,
	kCGSEventNotificationOtherMouseUp,
	kCGSEventNotificationOtherMouseDragged,

	kCGSEventNotificationZoom,

	kCGSEventNotificationAppIsUnresponsive,
	kCGSEventNotificationAppIsNoLongerUnresponsive,

	kCGSEventSecureTextInputIsActive,
	kCGSEventSecureTextInputIsOff,

	kCGSEventNotificationSymbolicHotKeyChanged,
	kCGSEventNotificationSymbolicHotKeyDisabled,
	kCGSEventNotificationSymbolicHotKeyEnabled,
	kCGSEventNotificationHotKeysGloballyDisabled,
	kCGSEventNotificationHotKeysGloballyEnabled,
	kCGSEventNotificationHotKeysExceptUniversalAccessGloballyDisabled,
	kCGSEventNotificationHotKeysExceptUniversalAccessGloballyEnabled,

	kCGSWindowIsObscured,
	kCGSWindowIsUnobscured,
	kCGSWindowIsOrderedIn,
	kCGSWindowIsOrderedOut,
	kCGSWindowIsTerminated,
	kCGSWindowIsChangingScreens,
	kCGSWindowDidMove,
	kCGSWindowDidResize,
	kCGSWindowDidChangeOrder,
	kCGSWindowGeometryDidChange,
	kCGSWindowMonitorDataPending,
	kCGSWindowDidCreate,
	kCGSWindowRightsGrantOffered,
	kCGSWindowRightsGrantCompleted,
	kCGSWindowRecordForTermination,
	kCGSWindowIsVisible,
	kCGSWindowIsInvisible,

	kCGSPackagesWorkspacesDisabled,
	kCGSPackagesWorkspacesEnabled,
	kCGSPackagesStatusBarSpaceChanged,

	kCGSessionConsoleConnect,
	kCGSessionConsoleDisconnect,
	kCGSessionRemoteConnect,
	kCGSessionRemoteDisconnect,
	kCGSessionLoggedOn,
	kCGSessionLoggedOff,
	kCGSessionConsoleWillDisconnect,

	kCGSTransitionDidFinish,

	kCGSConnectionWindowsBecameVisible,
	kCGSConnectionWindowsBecameOccluded,
	kCGSConnectionWindowModificationsStarted,
	kCGSConnectionWindowModificationsStopped,

	kCGSLikelyUnbalancedDisableUpdateNotification,

	kCGSWindowBecameVisible,
	kCGSWindowBecameOccluded,

	kCGSWorkspaceWillChange,
	kCGSWorkspaceDidChange,

	kCGSWorkspaceWindowIsViewable,
	kCGSWorkspaceWindowIsNotViewable,

	kCGSWorkspaceWindowDidMove,

	kCGSWorkspacePrefsDidChange,

	kCGSWorkspacesWindowDidOrderOutInOtherSpace,
	kCGSWorkspacesWindowDidOrderInOtherSpace,
	kCGSWorkspacesWindowDidChangeOrder,

	kCGSWorkspacesWindowDragDidStart,
	kCGSWorkspacesWindowDragDidEnd,

	kCGSWorkspacesWindowDidOrderInUserSpace,

	kCGSWorkspacesShowSpaceForProcess,

	kCGSWorkspacesWindowDidOrderInOnNonCurrentManagedSpacesOnly,
	kCGSWorkspacesWindowDidOrderOutOnNonCurrentManagedSpaces,
} CGSEventType;

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
	CGSEventRecordData data;    /* type-dependent data: 40 bytes */
	void *ioEventData;
	/*
	 unsigned short windowHeight;
	 unsigned short mainDisplayHeight;
	 unsigned short *unicodePayload;
	 unsigned int eventOwner;
	 unsigned char passedThrough;
	 */
	unsigned int _field15[4];
	unsigned short _field16;
	unsigned short _field17;
	unsigned short *_field18;
} CGSEventRecord;

/// Gets the event record for a given `CGEventRef`.
///
/// For Carbon events, use `GetEventPlatformEventRecord`.
CG_EXTERN CGError CGEventGetEventRecord(CGEventRef event, CGSEventRecord *outRecord, size_t recSize);

#endif /* GSPRIVATE_H */
