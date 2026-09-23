add_executable(CollisionRegressionTest
    collision_correction_regression.cpp
)

target_link_libraries(CollisionRegressionTest PRIVATE Special-Spoon)

target_include_directories(CollisionRegressionTest PRIVATE
    ${CMAKE_SOURCE_DIR}/Special-Spoon/src
    ${CMAKE_SOURCE_DIR}/Special-Spoon/vendor
    ${CMAKE_BINARY_DIR}/Special-Spoon
)

add_test(NAME collision_correction_regression COMMAND CollisionRegressionTest)
