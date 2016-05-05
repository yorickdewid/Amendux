// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SHELLMOD_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SHELLMOD_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef SHELLMOD_EXPORTS
#define SHELLMOD_API __declspec(dllexport)
#else
#define SHELLMOD_API __declspec(dllimport)
#endif

extern "C" {

	// This class is exported from the ShellMod.dll
	class SHELLMOD_API CShellMod {
	  public:
		CShellMod();
		// TODO: add your methods here.
	};

	extern SHELLMOD_API int nShellMod;

	SHELLMOD_API int fnShellMod();
}
