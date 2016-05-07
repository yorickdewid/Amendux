// DoSMod.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "DoSMod.h"


// This is an example of an exported variable
DOSMOD_API int nDoSMod=0;

// This is an example of an exported function.
DOSMOD_API int fnDoSMod(void)
{
    return 42;
}

// This is the constructor of a class that has been exported.
// see DoSMod.h for the class definition
CDoSMod::CDoSMod()
{
    return;
}
