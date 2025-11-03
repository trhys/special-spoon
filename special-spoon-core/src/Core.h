//Preprocessor definitions
#pragma once

#ifdef SS_PLATFORM_WINDOWS
    #ifdef SS_BUILD_DLL
        #define SPOON_API __declspec(dllexport)
    #else
        #define SPOON_API __declspec(dllimport)
    #endif
#else
    #error Platform not supported!
#endif