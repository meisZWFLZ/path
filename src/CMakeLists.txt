project(library)

# All sources that also need to be tested in unit tests go into a static library
add_library(path_file_system STATIC pathFileSystem.cpp)
add_library(bytebuffer STATIC byteBuffer.cpp)
# set_target_properties(path_file_system PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
target_include_directories(path_file_system PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(bytebuffer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# The main program
add_executable(main_program main.cpp)
# set_target_properties(server PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
target_link_libraries(main_program PRIVATE path_file_system bytebuffer pthread)