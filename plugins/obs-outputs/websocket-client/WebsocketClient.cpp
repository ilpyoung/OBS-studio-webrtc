/* Copyright Dr. Alex. Gouaillard (2015, 2020) */

#include <openssl/opensslv.h>
// #include "MillicastWebsocketClientImpl.h"
#include "CustomWebrtcImpl.h"

WEBSOCKETCLIENT_API WebsocketClient * createWebsocketClient(void)
{
	return new CustomWebrtcImpl();
}
