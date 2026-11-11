#pragma once

#include "Core/Renderer/Renderer.h"
#include "Core/EntityManager/EntityManager.h"
#include "System/SystemManager.h"
#include "Core/SceneManager.h"
#include "Core/Project/ProjectManager.h"
#include "System/Core/InputSystem.h"
#include "Utils/Metrics/Metrics.h"

namespace Spoon
{
    struct Services
    {
        Renderer* renderer = nullptr;
        EntityManager* entityManager = nullptr;
        SystemManager* systemManager = nullptr;
        SceneManager* sceneManager = nullptr;
        ProjectManager* projectManager = nullptr;
        InputSystem* inputSystem = nullptr;
        Metrics* metrics = nullptr;
    };
}