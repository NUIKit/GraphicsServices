//
//  CFBase.c
//  GraphicsServices
//
//  Created by Robert Widmann on 7/31/15.
//  Copyright © 2015 CodaFi. All rights reserved.
//

#include "GSPrivate.h"

const CFStringRef kGSEventReceiveRunLoopMode = CFSTR("GSEventReceiveRunLoopMode");

void GSInitialize() {
	static bool initOnce;
	if (initOnce == false) {
		_GSEventClassInitialize();
		initOnce = true;
	}
}
