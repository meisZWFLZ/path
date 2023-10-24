project(library_tests)

find_package(Catch2 3 REQUIRED)

# The test program
add_executable(tests testmain.cpp)
target_link_libraries(tests PRIVATE path_file_system Catch2::Catch2WithMain pthread)