// Detect the platform
#ifndef PLATFORM
	#define PLATFORM_WINDOWS  1
	#define PLATFORM_MAC      2
	#define PLATFORM_UNIX     3

	#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__WIN32__) || defined(__MINGW32__) || defined(WINNT) || defined(__WINNT) || defined(__WINNT__)
		#define PLATFORM PLATFORM_WINDOWS
	#elif defined(__linux) || defined(__linux__) || defined(unix) || defined(__unix) || defined(__unix__)
		#define PLATFORM PLATFORM_UNIX
	#elif defined(_mac)
		#define PLATFORM PLATFORM_MAC
	#endif
#endif
