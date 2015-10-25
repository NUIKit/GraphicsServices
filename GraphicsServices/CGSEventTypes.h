//
//  GSEventTypes.h
//  GraphicsServices
//
//  Created by Robert Widmann on 10/25/15.
//  Copyright © 2015 CodaFi. All rights reserved.
//

#ifndef GSEVENT_TYPES_H
#define GSEVENT_TYPES_H

#include "GSEventTypes.def"

typedef enum {
#define X(EVT, VAL) EVT = VAL,
	GS_EVENT_TYPES_TABLE
#undef X
} CGSEventType;


#endif /* GSEVENT_TYPES_H */
