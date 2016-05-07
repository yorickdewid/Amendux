// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DOSMOD_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DOSMOD_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef DOSMOD_EXPORTS
#define DOSMOD_API __declspec(dllexport)
#else
#define DOSMOD_API __declspec(dllimport)
#endif

// This class is exported from the DoSMod.dll
class DOSMOD_API CDoSMod {
public:
	CDoSMod(void);
	// TODO: add your methods here.
};

extern DOSMOD_API int nDoSMod;

DOSMOD_API int fnDoSMod(void);
