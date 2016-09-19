#include "stdafx.h"
#include "AddrLookup.h"
#include <cstdio>

using namespace SimpleSocksPrivateNS;

AddrLookup::AddrLookup() : list(NULL), node(NULL)
{
	//NOP
}

AddrLookup::~AddrLookup()
{
	clear();
}

int AddrLookup::lookup(const char* host, unsigned short port, int protocol)
{
	clear();

	//(I hate GAI. It's cumbersome and using it feels nasty.
	//I mean it's better than gethostbyname() was, but jeez...
	//One time I got so sick of it that I wrote my own DNS
	//resolver, mostly as an exercise. It ended up being pretty
	//much like what gethostbyname() used to be except it just
	//returned the first net-order address long from the list.
	//Ah... Good times....)
	char portstr[10];
	sprintf_s(portstr, "%u", port);

	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_protocol = protocol;

	int retval = getaddrinfo(host, portstr, &hints, &list);

	//We need to retain the root in 'list' for
	//freeaddrinfo(), so 'node' is used for traversal.
	//(Also allows possiblity for a 'rewind()' to be added.)
	node = list;

	return retval;
}

sockaddr *AddrLookup::getNext()
{
	if (!node) {
		return NULL;
	}

	sockaddr *retval = node->ai_addr;

	node = node->ai_next;

	return retval;
}

void AddrLookup::clear() {
	freeaddrinfo(list);
	list = NULL;
	node = NULL;
}
