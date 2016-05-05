// ShellMod.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ShellMod.h"


// This is an example of an exported variable
SHELLMOD_API int nShellMod = 0;

// This is an example of an exported function.
SHELLMOD_API int fnShellMod()
{
	MessageBox(NULL, L"Hello World", L"In a DLL", MB_OK);

    return 42;
}

// This is the constructor of a class that has been exported.
// see ShellMod.h for the class definition
CShellMod::CShellMod()
{
    return;
}
