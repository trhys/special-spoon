//Preprocessor definitions
#pragma once

#include <iostream>

#ifdef SS_PLATFORM_WINDOWS
    #ifdef SS_BUILD_DLL
        #define SPOON_API __declspec(dllexport)
    #else
        #define SPOON_API __declspec(dllimport)
    #endif
#else
    #error Platform not supported!
#endif

//Debug Utils

#define SS_CPROF_STARTUP "**STARTUP**"

#ifdef SS_DEBUG_ENABLED
    #define SS_TRY try {
    #define SS_CATCH(ss_catch_prof) } \
        catch (const std::bad_alloc& e) \
            { \
                std::cerr << "Fatal error: Bad memory allocation at: " << ss_catch_prof << " --- Details: " << e.what() << std::endl \
                delete app; \
                return EXIT_FAILURE; \
            } \
        catch (const std::exception& e) \
            { \
                std::cerr << "Fatal error: An unexpected error occured at: " << ss_catch_prof << " --- Details: " << e.what() << std::endl \
                delete app; \
                return EXIT_FAILURE; \
            }
#else
    #define SS_TRY
    #define SS_CATCH_BAD_ALLOC(ss_catch_prof)

#ifdef SS_ASSERTS_ENABLED
    #define SS_INSTANCE_ASSERT(s_Instance) assert(!s_Instance && "Application already exists!");