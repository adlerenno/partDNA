cmake_minimum_required(VERSION 3.18)

project(bwt VERSION 1.0.0 DESCRIPTION "Project converting different bwt representations")

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl")

option(ENABLE_TIME_EVALUATION "Split time needed for construction" ON)
option(ENABLE_VERBOSE "Outputs information about steps and progress" ON)

# configure_file( fehlt hier noch )
# get files with "find bits bwt collections genomeassembly io splitting suffxarrayconstruction util -type f -name '*.c' -o -name '*.h' | sort"
set(SOURCES
        main.c
        collections/intlist.c
        collections/intlist.h
        collections/list.c
        collections/list.h
        collections/listutil.c
        collections/listutil.h
        io/kseq.h
        io/raw_input.c
        io/raw_input.h
        io/raw_input_dna.c
        io/raw_input_dna.h
        io/raw_output_dna.c
        io/raw_output_dna.h
        splitting/splitting_dna.c
        splitting/splitting_dna.h
        util/constants.h
        util/eval/time_consumption.c
        util/eval/time_consumption.h
        util/panic.c
        util/panic.h
        util/trap.h
)

if(ENABLE_TIME_EVALUATION)
        add_definitions(-DTIME_EVALUATION)
endif()

if(ENABLE_VERBOSE)
        add_definitions(-DVERBOSE)
endif()

set(INCLUDES
        ${CMAKE_CURRENT_BINARY_DIR}
        bits
        splitting
        collections
        util
        /usr/local/lib/
        /usr/local/include/
        )

add_library(${PROJECT_NAME} SHARED ${SOURCES})
find_package(ZLIB)
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDES})
target_link_libraries(${PROJECT_NAME} PRIVATE m) # link with math library
target_link_libraries(${PROJECT_NAME} PRIVATE ZLIB::ZLIB)

if(NOT EXISTS "/usr/local/lib/libsais64.a")
    message(FATAL_ERROR "libsais was not found on the standart path /usr/local/lib/libsais64.a. Use the following bash steps to install the dependency.
    wget https://web.archive.org/web/20230309123010/https://sites.google.com/site/yuta256/sais-2.4.1.zip
    unzip sais-2.4.1.zip
    cd sais-2.4.1
    mkdir -p build
    cmake -DCMAKE_BUILD_TYPE=\"Release\" -DCMAKE_INSTALL_PREFIX=\"usr/local/lib/\" -DBUILD_EXAMPLES=OFF -DBUILD_SAIS64=ON -DBUILD_SHARED_LIBS=OFF ..
    make
    sudo make install")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    target_link_libraries(${PROJECT_NAME} PRIVATE /usr/local/lib/libsais64.dylib)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE sais64)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER bwt.h)

# Creating a dynamic shared library.

#include(GNUInstallDirs)
#install(TARGETS bwt
#        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Create Executeable
add_executable(exc main.c)
add_dependencies(exc ${PROJECT_NAME}) # add library add dependency

target_include_directories(exc PRIVATE ${INCLUDES})

target_link_libraries(exc PRIVATE ${PROJECT_NAME})
