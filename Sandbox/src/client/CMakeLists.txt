target_sources(Sandbox
    PRIVATE
        init.cpp
    PUBLIC
        FILE_SET HEADERS
        FILES
            CustomCompLoader.h
            CustomSystemLoader.h
)

# add any custom code into the sandbox build here

# EXAMPLE:
# add_subdirectory(ECS)