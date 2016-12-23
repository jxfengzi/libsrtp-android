CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(stream C)

SET(PROJ_NAME "stream")

#---------------------------------------------------------------------------------------
#
# c flags
#
#---------------------------------------------------------------------------------------
ADD_DEFINITIONS(-DTINY_DEBUG)

#---------------------------------------------------------------------------------------
# WIN32
#---------------------------------------------------------------------------------------
IF(WIN32)

    MESSAGE(STATUS "build ${PROJ_NAME}for Windows")

    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
    ADD_DEFINITIONS(-DUNICODE)
    ADD_DEFINITIONS(-D_UNICODE)

ELSE(WIN32)

    ADD_DEFINITIONS(-Wno-multichar)

    # release
    ADD_DEFINITIONS(-O3)

    # debug
    #ADD_DEFINITIONS(-g)

    ADD_DEFINITIONS(-fPIC)

    #-----------------------------------------------------------------------------------
    # APPLE MACOSX
    #-----------------------------------------------------------------------------------
    IF(APPLE)
        MESSAGE(STATUS "build ${PROJ_NAME} for osx")
        ADD_DEFINITIONS(-D__MAC_OSX__)

        # Apple LLVM Compiler
        # SET(CMAKE_CXX_COMPILER /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++)

        # LLVM gcc Compiler
        SET(CMAKE_CXX_COMPILER /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++)
        SET(CMAKE_CXX_FLAGS -m64)

        #SET(CMAKE_CXX_COMPILER g++)
        #SET(CMAKE_CXX_FLAGS -m64)

    #-----------------------------------------------------------------------------------
    # Linux
    #-----------------------------------------------------------------------------------
    ELSE(APPLE)
        MESSAGE(STATUS "build ${PROJ_NAME} for Linux")
        ADD_DEFINITIONS(-D__LINUX__)
    ENDIF(APPLE)
ENDIF(WIN32)

#---------------------------------------------------------------------------------------
#
# include dir
#
#---------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/libsrtp/include)

#--------------------------------------------------------------------------------------
#
# lib
#
#--------------------------------------------------------------------------------------
IF (WIN32)
    SET(OS_LIB)
ELSE (WIN32)
    IF(APPLE)
        SET(OS_LIB)
    ELSE(APPLE)
        SET(OS_LIB)
    ENDIF(APPLE)
ENDIF (WIN32)

MESSAGE(STATUS "os_lib: ${OS_LIB}")

#-------------------------------------------------------------------------------------
# set output dir
#-------------------------------------------------------------------------------------
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/output)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/output)
MESSAGE(STATUS "executable output:  ${EXECUTABLE_OUTPUT_PATH}")
MESSAGE(STATUS "library output: ${LIBRARY_OUTPUT_PATH}")

#-------------------------------------------------------------------------------------
#
# project
#
#-------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(src)
