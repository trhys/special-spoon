target_sources(Sandbox
    PRIVATE
        Component/Combat/AttackComp.cpp

    PUBLIC
        FILE_SET HEADERS
        FILES
            Component/Combat/AttackComp.h
            Component/Combat/HealthComp.h
            Component/Combat/Attack.h
            Component/Combat/HurtboxComp.h
            Component/Combat/RespawnComp.h
        
            System/CombatSystem.h
            System/HealthSystem.h

            CustomCompLoader.h
            CustomSystemLoader.h
)