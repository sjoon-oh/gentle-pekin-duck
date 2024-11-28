cmake_minimum_required(VERSION 3.10)
project(pduck)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE true)

add_compile_options(
    "-Wall" "-Wextra" "$<$<CONFIG:DEBUG>:-O0;-g3;-ggdb>"
)
# cmake . -DCMAKE_BUILD_TYPE=Debug

set(CMAKE_BUILD_TYPE Debug)

# Include boost library
find_package(
    Boost 1.80 REQUIRED 
    COMPONENTS program_options
)
# if (Boost_FOUND)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIR})

set(Boost_USE_STATIC_LIBS ON)
# endif()

# Add submodules
# add_subdirectory(extern/YCSB-C)

# External sources
include_directories(${PROJECT_SOURCE_DIR}/extern/spdlog/include)

# Internal sources
# include_directories(${PROJECT_SOURCE_DIR}/inc)
file(GLOB_RECURSE HDR_FILES 
    CONFIGURE_DEPENDS
    inc/*.h inc/*.hh
    )
file(GLOB_RECURSE SRC_FILES
    CONFIGURE_DEPENDS
    src/*.c src/*.cc
    )


set(PDUCKLIB pduck)
add_library(${PDUCKLIB} 
    SHARED ${HDR_FILES} ${SRC_FILES}
)
set_target_properties(${PDUCKLIB} 
    PROPERTIES 
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/lib
    LINKER_LANGUAGE CXX
)
target_include_directories(${PDUCKLIB} 
    PUBLIC ${PROJECT_SOURCE_DIR}/inc
)
target_link_libraries(${PDUCKLIB} 
    PUBLIC pthread ${Boost_LIBRARIES}
)

# Test binary
option(TEST_BINARY "TEST-BINARY" ON)
if (TEST_BINARY)

    # Simple test
    add_executable(
        pduck-start ${HDR_FILES} ${SRC_FILES} ${PROJECT_SOURCE_DIR}/test/start.cc)
    set_target_properties(
        pduck-start PROPERTIES 
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/bin
        LINKER_LANGUAGE CXX
    )
    target_include_directories(pduck-start PUBLIC ${PROJECT_SOURCE_DIR}/inc)
    target_link_libraries(pduck-start PUBLIC ${Boost_LIBRARIES} ${PDUCKLIB})
    add_dependencies(pduck-start ${Boost_LIBRARIES} ${PDUCKLIB})

    # cache test
    add_executable(
        pduck-cache ${HDR_FILES} ${SRC_FILES} ${PROJECT_SOURCE_DIR}/test/cache.cc)
    set_target_properties(
        pduck-cache PROPERTIES 
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/bin
        LINKER_LANGUAGE CXX
    )
    target_include_directories(pduck-cache PUBLIC ${PROJECT_SOURCE_DIR}/inc)
    target_link_libraries(pduck-cache PUBLIC ${Boost_LIBRARIES} ${PDUCKLIB})
    add_dependencies(pduck-cache ${Boost_LIBRARIES} ${PDUCKLIB})

    # cache-lfu test
    add_executable(
        pduck-cache-lfu ${HDR_FILES} ${SRC_FILES} ${PROJECT_SOURCE_DIR}/test/cache-lfu.cc)
    set_target_properties(
        pduck-cache-lfu PROPERTIES 
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/bin
        LINKER_LANGUAGE CXX
    )
    target_include_directories(pduck-cache-lfu PUBLIC ${PROJECT_SOURCE_DIR}/inc)
    target_link_libraries(pduck-cache-lfu PUBLIC ${Boost_LIBRARIES} ${PDUCKLIB})
    add_dependencies(pduck-cache-lfu ${Boost_LIBRARIES} ${PDUCKLIB})

endif()
