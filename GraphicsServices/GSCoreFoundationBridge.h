//
//  GSCoreFoundationBridge.h
//  GraphicsServices
//
//  Created by Robert Widmann on 7/31/15.
//  Copyright © 2015 CodaFi. All rights reserved.
//

#ifndef GSCOREFOUNDATIONBRIDGE_H
#define GSCOREFOUNDATIONBRIDGE_H

enum {
	_kCFRuntimeNotATypeID = 0
};

typedef struct __CFRuntimeBase {
	uintptr_t _cfisa;
	uint8_t _cfinfo[4];
#if __LP64__
	uint32_t _rc;
#endif
} CFRuntimeBase;

typedef struct __CFRuntimeClass {	// Version 0 struct
	CFIndex version;
	const char *className;
	void (*init)(CFTypeRef cf);
	CFTypeRef (*copy)(CFAllocatorRef allocator, CFTypeRef cf);
#if MAC_OS_X_VERSION_10_2 <= MAC_OS_X_VERSION_MAX_ALLOWED
	void (*finalize)(CFTypeRef cf);
#else
	void (*dealloc)(CFTypeRef cf);
#endif
	Boolean (*equal)(CFTypeRef cf1, CFTypeRef cf2);
	CFHashCode (*hash)(CFTypeRef cf);
	CFStringRef (*copyFormattingDesc)(CFTypeRef cf, CFDictionaryRef formatOptions);	// str with retain
	CFStringRef (*copyDebugDesc)(CFTypeRef cf);	// str with retain
#if MAC_OS_X_VERSION_10_5 <= MAC_OS_X_VERSION_MAX_ALLOWED
#define CF_RECLAIM_AVAILABLE 1
	void (*reclaim)(CFTypeRef cf);
#endif
} CFRuntimeClass;

CF_EXPORT CFTypeID _CFRuntimeRegisterClass(const CFRuntimeClass * const cls);
CF_EXPORT CFTypeRef _CFRuntimeCreateInstance(CFAllocatorRef allocator, CFTypeID typeID, CFIndex extraBytes, unsigned char *category);

#endif /* GSCOREFOUNDATIONBRIDGE_H */
