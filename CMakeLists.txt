cmake_minimum_required(VERSION 3.28)

project(bnpython C)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_C_LINK_LIBRARY_USING_weak_library "PATH{-weak_library <LIBRARY>}NAME{LINKER:-weak-l<LIB_ITEM>}")
set(CMAKE_C_LINK_LIBRARY_USING_weak_library_SUPPORTED TRUE)

set(BN_PYTHON_DYLIB "/Applications/Binary Ninja.app/Contents/Frameworks/Python.framework/Versions/3.10/Python")
set(PYTHON_HEADER_PATH "/Library/Frameworks/Python.framework/Versions/3.10/include/python3.10")

include_directories(SYSTEM ${PYTHON_HEADER_PATH})

add_executable(bnpython bnpython.c shared.c)
#target_link_libraries(bnpython "$<LINK_LIBRARY:weak_library,${BN_PYTHON_DYLIB}>")

#add_executable(bnpip bnpip.c shared.c)
#target_link_libraries(bnpip ${BN_PYTHON_DYLIB})

#add_executable(bnpython-config bnpython-config.c shared.c)
#target_link_libraries(bnpython-config ${BN_PYTHON_DYLIB})