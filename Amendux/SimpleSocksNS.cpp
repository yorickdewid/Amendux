#include "stdafx.h"
#include "SimpleSocksNS.h"
#include "WSASystemMgr.h"

//Note that the object exists here whether init is called or not.
//Originally I didn't even have the init/shutdown funcs and just
//did it all in ctor/dtor, but then I figured someone may want
//to try using a different winsock version (freak) so I made it
//like this.
//Anyway, the dtor will ensure that cleanup is called if the thing
//crashes or doesn't get shut down. The shutdown func is really
//just there in case someone wants to shut down winsock in mid
//program for some reason.c
static SimpleSocksPrivateNS::WSASystemMgr simpleSocksPrivateWSAobject;

int SimpleSocks::initSimpleSocks() {
	return simpleSocksPrivateWSAobject.start();
}

void SimpleSocks::shutdownSimpleSocks() {
	simpleSocksPrivateWSAobject.stop();
}
