/*
 * server_tianqin.h
 *
 *  Created on: 2015/6/25
 *      Author: jk
 */

#ifndef SRC_SERVER_TIANQIN_H_
#define SRC_SERVER_TIANQIN_H_

#include <event2/event.h>

struct evconnlistener* server_tianqin(struct event_base*, int);

#endif /* SRC_SERVER_TIANQIN_H_ */
