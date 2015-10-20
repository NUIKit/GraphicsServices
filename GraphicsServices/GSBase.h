//
//  GSBase.h
//  GraphicsServices
//
//  Created by Robert Widmann on 7/31/15.
//  Copyright © 2015 CodaFi. All rights reserved.
//

#ifndef GSBASE_H
#define GSBASE_H

#include <CoreFoundation/CoreFoundation.h>

/// Sets up initial state.
///
/// This function should be invoked before interacting with any other part of GraphicsServices.
CF_EXPORT void GSInitialize(void);

#endif
