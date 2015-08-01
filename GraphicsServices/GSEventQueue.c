//
//  GSEventQueue.c
//  GraphicsServices
//
//  Created by Robert Widmann on 8/1/15.
//  Copyright © 2015 CodaFi. All rights reserved.
//

#include "GSEventQueue.h"
#include <pthread.h>

static pthread_once_t GSThreadLocalContextOnce = PTHREAD_ONCE_INIT;
static pthread_key_t GSThreadLocalContextKey;

static void GSFreeContextStack(void *queue) {
	GSPurpleQueueRef queueref = (GSPurpleQueueRef)queue;
	while (!GSEventQueueIsEmpty(queueref)) {
		GSEventQueuePopEvent();
	}
	free(queueref);
}

static void GSCreateContextOnceKey() {
	pthread_key_create(&GSThreadLocalContextKey, GSFreeContextStack);
}

GSPurpleQueueRef GSEventQueueGetThreadLocalQueue() {
	pthread_once(&GSThreadLocalContextOnce, GSCreateContextOnceKey);
	GSPurpleQueueRef contexts = pthread_getspecific(GSThreadLocalContextKey);
	if (contexts == NULL) {
		GSPurpleQueueRef queue;
		
		queue = malloc(sizeof(*queue));
		queue->front = queue->back = NULL;
		pthread_setspecific(GSThreadLocalContextKey, queue);
		
		return queue;
	}
	return contexts;
}

bool GSEventQueueIsEmpty(GSPurpleQueueRef queue) {
	return (queue == NULL) || (queue->front == NULL);
}

GSEventRef GSEventQueueGetCurrentEvent() {
	GSPurpleQueueRef queue = GSEventQueueGetThreadLocalQueue();
	if (queue->front == NULL) {
		return NULL;
	}
	return queue->front->event;
}

void GSEventQueuePopEvent() {
	GSPurpleQueueRef queue = GSEventQueueGetThreadLocalQueue();
	GSPurpleEventPairRef deadNode = queue->front;
	
	if (queue->front == NULL) {
		return;
	}
	
	if (queue->front == queue->back) {
		queue->front = queue->back = NULL;
	} else {
		queue->front = queue->front->next;
	}
	
	if (deadNode->event != NULL) {
		CFRelease(deadNode->event);
	}
	free(deadNode);
}

void GSEventQueuePushEvent(GSEventRef event) {
	GSPurpleQueueRef queue = GSEventQueueGetThreadLocalQueue();
	GSPurpleEventPairRef pair;
	
	pair = malloc(sizeof(*pair));
	pair->event = (GSEventRef)CFRetain(event);
	pair->next = NULL;
	
	if (queue->front == NULL) {
		queue->front = queue->back = pair;
	} else {
		queue->back->next = pair;
		queue->back = pair;
	}
}
