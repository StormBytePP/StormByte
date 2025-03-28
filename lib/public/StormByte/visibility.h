#pragma once

#include <StormByte/platform.h>

#ifdef WINDOWS
	#ifdef StormByte_EXPORTS
		#define STORMBYTE_PUBLIC	__declspec(dllexport)
  	#else
      	#define STORMBYTE_PUBLIC	__declspec(dllimport)
  	#endif
  	#define STORMBYTE_PRIVATE
#else
    #define STORMBYTE_PUBLIC		__attribute__ ((visibility ("default")))
    #define STORMBYTE_PRIVATE		__attribute__ ((visibility ("hidden")))
#endif
