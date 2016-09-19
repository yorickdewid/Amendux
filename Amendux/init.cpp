//
//  UNIT: init.cpp 
//
//  PURPOSE: Run self-modifying routines 
//

#include "stdafx.h"

EXTERN_C int APIENTRY wWinMainCRTStartup(void);

int _veinit() {

	// Initialize CRT
	return wWinMainCRTStartup();
}

