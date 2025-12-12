#pragma once

#include "Utils/Logger.h"
#include <iostream>
#include "special-spoon-core_export.h"

#define SPOON_API SPECIAL_SPOON_CORE_EXPORT

//Debug Utils
#ifdef SS_DEBUG_ENABLED
    #define SS_DEBUG_LOG(msg) Spoon::Logger::Get().Write(std::string(msg));
#else
    #define SS_DEBUG_LOG(msg)
#endif

#ifdef SS_ASSERTS_ENABLED
    #define SS_INSTANCE_ASSERT(s_Instance) assert(!s_Instance && "Application already exists!");
#else 
    #define SS_INSTANCE_ASSERT(s_Instance)
#endif

// Memory utils

#ifdef SS_MEMORY_ENABLED
   #define SS_MUTILS_ON
#endif