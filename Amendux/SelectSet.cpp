#include "stdafx.h"
#include "SelectSet.h"
#include <WinSock2.h>

SimpleSocks::SelectSet::SelectSet() : pimpl(NULL)
{
	//NOP
}

SimpleSocks::SelectSet::~SelectSet()
{
	//NOP
}

int SimpleSocks::SelectSet::copyFrom(SimpleSocks::SelectSet& src)
{
	return copyFrom(&src);
}

int SimpleSocks::SelectSet::copyFrom(SimpleSocks::SelectSet* src)
{
	if (src->pimpl.get() == NULL) {
		pimpl.reset(NULL);
		return 0;
	}
	std::auto_ptr<impl> timpl;
	try { timpl.reset(new impl(src->pimpl.get())); }
	catch (std::bad_alloc) { return -5; }

	pimpl.reset(timpl.release());
	return 0;
}

int SimpleSocks::SelectSet::getCount() const
{
	if (pimpl.get() == NULL) { return 0; }
	return (int)(pimpl->tcps.size() + pimpl->srvs.size() + pimpl->udps.size());
}

void SimpleSocks::SelectSet::clear() {
	pimpl.reset(NULL);
}

int SimpleSocks::SelectSet::validate() {
	if (pimpl.get() == NULL) {
		return 0;
	}

	//Basically we're creating a new pimpl here and then iterating
	//through the maps in the existing pimpl and copying over all
	//valid pairs. Once that's done we just replace the pimpl with
	//the new one. Easy-peasy. Also, I don't use algorithms for
	//this kind of thing because it needlessly separates the code
	//and generally does not provide any kind of improvement.
	std::auto_ptr<impl> timpl;

	std::map<SOCKET, TCPSocket*>::iterator tcpIter, tcpLast;
	tcpLast = pimpl->tcps.end();
	for (tcpIter = pimpl->tcps.begin(); tcpIter != tcpLast; ++tcpIter) {
		TCPSocket *sock = tcpIter->second;
		if (sock->isConnected() && (sock->getBlocking() == false)) {
			timpl->tcps[sock->pimpl->sock] = sock;
		}
	}

	std::map<SOCKET, TCPServer*>::iterator srvIter, srvLast;
	srvLast = pimpl->srvs.end();
	for (srvIter = pimpl->srvs.begin(); srvIter != srvLast; ++srvIter) {
		TCPServer *serv = srvIter->second;
		if (serv->isRunning() && (serv->getBlocking() == false)) {
			timpl->srvs[serv->pimpl->serv] = serv;
		}
	}

	std::map<SOCKET, UDPSocket*>::iterator udpIter, udpLast;
	udpLast = pimpl->udps.end();
	for (udpIter = pimpl->udps.begin(); udpIter != udpLast; ++udpIter) {
		UDPSocket* sock = udpIter->second;
		if ((sock->getMode() != UDPSocket::UDP_INACTIVE) && (sock->getBlocking() == false)) {
			timpl->udps[sock->pimpl->sock] = sock;
		}
	}

	pimpl.reset(timpl.release());

	return getCount();
}

int SimpleSocks::SelectSet::pushObject(SimpleSocks::TCPSocket* sock) {
	if (sock->isConnected() == false) { return -1; }
	if (sock->getBlocking()) { return -1; }

	if (pimpl.get() == NULL) {
		try { pimpl.reset(new impl); }
		catch (std::bad_alloc) { return -5; }
	}

	pimpl->tcps[sock->pimpl->sock] = sock;
	return 0;
}

int SimpleSocks::SelectSet::pushObject(SimpleSocks::TCPServer* serv) {
	if (serv->isRunning() == false) { return -1; }
	if (serv->getBlocking()) { return -1; }

	if (pimpl.get() == NULL) {
		try { pimpl.reset(new impl); }
		catch (std::bad_alloc) { return -5; }
	}

	pimpl->srvs[serv->pimpl->serv] = serv;
	return 0;
}

int SimpleSocks::SelectSet::pushObject(SimpleSocks::UDPSocket* sock) {
	if (sock->getMode() == UDPSocket::UDP_INACTIVE) { return -1; }
	if (sock->getBlocking()) { return -1; }

	if (pimpl.get() == NULL) {
		try { pimpl.reset(new impl); }
		catch (std::bad_alloc) { return -5; }
	}

	pimpl->udps[sock->pimpl->sock] = sock;
	return 0;
}

void SimpleSocks::SelectSet::removeObject(TCPSocket* sock) {
	if (pimpl.get() == NULL) { return; }

	std::map<SOCKET, TCPSocket*>::iterator iter, last;
	last = pimpl->tcps.end();
	//Wouldn't it be awesome if map::find() searched for an element
	//instead of a key? Durrrrr....
	for (iter = pimpl->tcps.begin(); iter != last; ++iter) {
		if (iter->second == sock) {
			pimpl->tcps.erase(iter);
			break;
		}
	}
}

void SimpleSocks::SelectSet::removeObject(TCPServer* serv) {
	if (pimpl.get() == NULL) { return; }

	std::map<SOCKET, TCPServer*>::iterator iter, last;
	last = pimpl->srvs.end();
	for (iter = pimpl->srvs.begin(); iter != last; ++iter) {
		if (iter->second == serv) {
			pimpl->srvs.erase(iter);
			break;
		}
	}
}

void SimpleSocks::SelectSet::removeObject(UDPSocket* sock) {
	if (pimpl.get() == NULL) { return; }

	std::map<SOCKET, UDPSocket*>::iterator iter, last;
	last = pimpl->udps.end();
	for (iter = pimpl->udps.begin(); iter != last; ++iter) {
		if (iter->second == sock) {
			pimpl->udps.erase(iter);
			break;
		}
	}
}

SimpleSocks::TCPSocket* SimpleSocks::SelectSet::popTCPSocket() {
	if (pimpl.get() == NULL) { return NULL; }
	if (pimpl->tcps.empty()) { return NULL; }
	std::map<SOCKET, TCPSocket*>::iterator iter = pimpl->tcps.begin();
	TCPSocket* retval = iter->second;
	pimpl->tcps.erase(iter);
	return retval;
}

SimpleSocks::TCPServer* SimpleSocks::SelectSet::popTCPServer() {
	if (pimpl.get() == NULL) { return NULL; }
	if (pimpl->srvs.empty()) { return NULL; }
	std::map<SOCKET, TCPServer*>::iterator iter = pimpl->srvs.begin();
	TCPServer* retval = iter->second;
	pimpl->srvs.erase(iter);
	return retval;
}

SimpleSocks::UDPSocket* SimpleSocks::SelectSet::popUDPSocket() {
	if (pimpl.get() == NULL) { return NULL; }
	if (pimpl->udps.empty()) { return NULL; }
	std::map<SOCKET, UDPSocket*>::iterator iter = pimpl->udps.begin();
	UDPSocket* retval = iter->second;
	pimpl->udps.erase(iter);
	return retval;
}

int SimpleSocks::select(SimpleSocks::SelectSet* set, unsigned int timeout) {
	if (set->getCount() == 0) { return -1; }

	//Here's another real 'winner' of antiquated functionality.
	//select() is such an enourmous pain in the butt that I've
	//even seen implementations that just always return the full set, lol.
	//Whatever. We just have to live with it until Vint Cerf and Al Gore
	//combine their internet powers and come to our rescue.
	fd_set testSet;
	FD_ZERO(&testSet);

	//Iterate through all 3 maps and add all socket handles to the sets.
	std::map<SOCKET, TCPSocket*>::iterator tcpIter, tcpLast;
	tcpLast = set->pimpl->tcps.end();
	for (tcpIter = set->pimpl->tcps.begin(); tcpIter != tcpLast; ++tcpIter) {
		FD_SET(tcpIter->first, &testSet);
	}
	std::map<SOCKET, TCPServer*>::iterator srvIter, srvLast;
	srvLast = set->pimpl->srvs.end();
	for (srvIter = set->pimpl->srvs.begin(); srvIter != srvLast; ++srvIter) {
		FD_SET(srvIter->first, &testSet);
	}
	std::map<SOCKET, UDPSocket*>::iterator udpIter, udpLast;
	udpLast = set->pimpl->udps.end();
	for (udpIter = set->pimpl->udps.begin(); udpIter != udpLast; ++udpIter) {
		FD_SET(udpIter->first, &testSet);
	}

	//run the selection
	int retval = -1;
	if (timeout == SELECT_FOREVER) {
		retval = ::select(0, &testSet, NULL, NULL, NULL);
	}
	else {
		timeval tv = { (long int) timeout / 1000, (timeout % 1000) * 1000 };
		retval = ::select(0, &testSet, NULL, NULL, &tv);
	}
	if (retval == SOCKET_ERROR) { return -1; }

	//And then rebuild all three sets from scratch, excluding anything
	//that's not in the result set. This is horrible and gross but I'll
	//rewrite it later after I've checked to ensure that the fd_set
	//internals are not subject to change.
	std::map<SOCKET, TCPSocket*> tcpReady;
	for (tcpIter = set->pimpl->tcps.begin(); tcpIter != tcpLast; ++tcpIter) {
		if (FD_ISSET(tcpIter->first, &testSet)) {
			tcpReady[tcpIter->first] = tcpIter->second;
		}
	}
	set->pimpl->tcps.swap(tcpReady);

	std::map<SOCKET, TCPServer*> srvReady;
	for (srvIter = set->pimpl->srvs.begin(); srvIter != srvLast; ++srvIter) {
		if (FD_ISSET(srvIter->first, &testSet)) {
			srvReady[srvIter->first] = srvIter->second;
		}
	}
	set->pimpl->srvs.swap(srvReady);

	std::map<SOCKET, UDPSocket*> udpReady;
	for (udpIter = set->pimpl->udps.begin(); udpIter != udpLast; ++udpIter) {
		if (FD_ISSET(udpIter->first, &testSet)) {
			udpReady[udpIter->first] = udpIter->second;
		}
	}
	set->pimpl->udps.swap(udpReady);

	return retval;
}

SimpleSocks::SelectSet::impl::impl() {
	//I was having some trouble with STL errors when I didn't
	//explicitly initialize the maps, so I added a ctor to impl
	//here, since I wanted to make the copy-by-pointer ctor
	//as well anyway.
	tcps.clear();
	srvs.clear();
	udps.clear();
}

SimpleSocks::SelectSet::impl::impl(impl* src) {
	tcps.clear();
	tcps.insert(src->tcps.begin(), src->tcps.end());
	srvs.clear();
	srvs.insert(src->srvs.begin(), src->srvs.end());
	udps.clear();
	udps.insert(src->udps.begin(), src->udps.end());
}
