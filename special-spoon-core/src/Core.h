#pragma once

#include <iostream>
#include "special-spoon-core_export.h"

#define SPOON_API SPECIAL_SPOON_CORE_EXPORT

//Debug Utils
#ifdef SS_DEBUG_ENABLED
    #define SS_TRY try {
    #define SS_CATCH(string) } \
        catch (const std::bad_alloc& e) \
            { \
                std::cerr << "Fatal error: Bad memory allocation during " << string << "--- Details: " << e.what() << std::endl; \
                delete app; \
                return EXIT_FAILURE; \
            } \
        catch (const std::exception& e) \
            { \
                std::cerr << "Fatal error: An unexpected error occured during startup --- Details: " << e.what() << std::endl; \
                delete app; \
                return EXIT_FAILURE; \
            }
#else
    #define SS_TRY
    #define SS_CATCH(string)
#endif

#ifdef SS_ASSERTS_ENABLED
    #define SS_INSTANCE_ASSERT(s_Instance) assert(!s_Instance && "Application already exists!");
#else 
    #define SS_INSTANCE_ASSERT(s_Instance)
#endif

// Memory utils

#ifdef SS_DEBUG_ENABLED
    #define SS_MUTILS_ON
#endif

// Resource Manager utility macros

#define GET_TEXTURE GetResource<sf::Texture>
#define GET_FONT GetResource<sf::Font>