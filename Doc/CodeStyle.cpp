//  Copyright (C) 2016 Amendux
//  All Rights Reserved.
//
//  UNIT: CodeStyle.cpp 
//
//  PURPOSE: Code style example
//
//  COMMENTS:
//
//        This unit does not compile,
//        nor does the code mean anything
//
//        Use standard C++ style and types,
//        avoid operating system specific code
//
//        Leave two spaces between function
//        declarations
//

// Include default headers
#include "stdafx.h"
#include "Log.h"
#include "Config.h"
// Unit specific header here
//#include "CodeSyle.h"

// Global includes here
//#include <activation.h>

using namespace Amendux;

/* Definitions */
#define DEFAULT_VAL		0
#define CONNECT_STAT	1
#define VERSION			"0.1"


/*
 * Class method description
 *
 * If the constructor does not
 * contain a body, move to class
 * header
 */
CodeStyle::CodeStyle()
{
	Object obj = new Object;
	if (!obj) {
		Log::Error(L"CodeStyle", L"Some error");
	}

	// Prefix std functions with ::
	::memset(obj, '\0', sizeof(Object));

	// Always use new,delete instead of malloc or LocalAlloc
	char *stuff = new char[16];
	if (!stuff) {
		Log::Error(L"CodeStyle", L"Allocation error");
		delete stuff;
	}

	delete[] stuff;

	/* Description */
	ClearSomething();
}


/*
 * Destructor does al the unloading,
 * cleanup, freeing and closing
 *
 * Multiline comments are spread
 * line this and never end with a dot
 */
CodeStyle::~CodeStyle()
{
	/* Cleanup resources */
	delete[] arr;
	delete object;
}


/*
 * Class method
 *
 * Methods use the uppercamelcase convention
 */
int CodeStyle::ReadStuff(char *buffer, unsigned int max)
{
	/*
	 * Indent like this
	 *
	 * Alwyas use the default case
	 */
	switch (type) {
		case Option::box1:
			strcpy_s(buffer, 256, "C");
			dbg = _READ;
			break;
		case Option::box2:
			strcpy_s(buffer, 256, "C++");
			dbg = _READ;
			break;
		case Option::box3:
		case Option::box4:
			strcpy_s(buffer, 256, "VC");
			dbg = _READ;
			break;
		case Option::box5:
		case Option::box6:
			strcpy_s(buffer, 256, "VC++");
			dbg = _READ;
			break;
		default:
			sprintf_s(nControl->response, 256, "Invalid open type %d\n", type);
			return 0;
	}

again:
	while (got < max) {
		retval = socket->recv(buffer, 1024);
		if (retval == 0) {
			break;
		} else if (retval < 1) {
			if (WSAGetLastError() == WSAEWOULDBLOCK) {
				if (got == 0) {
					wait(100);
					goto again;
				}
				break;
			}

			Log::Error(L"CodeStyle", L"Error");
			return 0;
		}
	}

	return retval;
}


/*
 * Return NULL on success
 */
void *CodeStyle::Write()
{
	Init();

	/* Declare object when used */
	Object *obj = Global::GetObject();
	if (obj && isFullMoon()) {
		fireAndForget();
	}

	/* Use booleans, not integers */
	isConditionMet = true;

	/* Eearly exit */
	if (!isConnected()) {
		return nullptr;
	}

	/*
	 * Integer status returns are always named 'retval'
	 * 
	 * Use C++ style casting
	 */
	int retval = pull(static_cast<char *>(buffer), 1024);
	if (retval < 1) {
		Log::Info(L"CodeStyle", L"All good here");
		return new Object;
	}

	/* Use nullptr, not NULL */
	return nullptr;
}


/*
 * One line functions are moved into the
 * class header and declared as inline
 */
inline void CodeStyle::SetMode(Mode mode)
{
	obj->mode = mode;
}


/*
 * Pure structure initializations are
 * moved into the header and declared
 * as inline
 */
inline void CodeStyle::ClearStuff()
{
	obj->mode = DEFAULT_VAL;
	obj->ctrl = nullptr;
	obj->cmode = 1;
	obj->xval = 0;
	obj->yval = 0;
	obj->zval = 0;
}


#if 0
/*
* Disabled code must not be out-commented
* but excluded from compilation
*/
void _test()
{
	CodeStyle cs;
	cs.Run();
	cs.Move();
	cs.Call();
}
#endif 
