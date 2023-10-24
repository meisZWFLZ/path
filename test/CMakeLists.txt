project(library_tests)

find_package(Catch2 3 REQUIRED)

# The test program
add_executable(tests testmain.cpp testByteBuffer.cpp)
target_link_libraries(tests PRIVATE path_file_system bytebuffer Catch2::Catch2WithMain pthread)