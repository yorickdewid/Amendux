//
//  UNIT: init.cpp 
//
//  PURPOSE: Run self-modifying routines and defines the entry point for the application.
//

#include "stdafx.h"

EXTERN_C int APIENTRY wWinMainCRTStartup(void);

int _veinit() {

	// Initialize CRT
	return wWinMainCRTStartup();
}

