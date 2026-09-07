cmake_minimum_required(VERSION 3.21...4.21)
project(GLapp)
set(TARGET GLapp)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET})

# set C++ version to use
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# I'm lazy here, and building with all cpp files in src
file(GLOB SOURCES  "src/*.cpp")
file(GLOB INCLUDES "src/*.hpp")
file(GLOB INLINES  "src/*.inl")
add_executable(${TARGET} ${SOURCES} ${INCLUDES} ${INLINES})

# set up Config.hpp to find directories
set(PROJECT_BASE_DIR "${PROJECT_SOURCE_DIR}")
set(PROJECT_DATA_DIR "${PROJECT_BASE_DIR}/data")
set(PROJECT_BUILD_DIR "${PROJECT_BINARY_DIR}")
configure_file(src/Config.hpp.in Config.hpp)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# add data files to solution explorer
file(GLOB DATA_FILES "data/*")
source_group("Data" FILES ${DATA_FILES})
target_sources(${TARGET} PRIVATE ${DATA_FILES})

# add GLFW, GLEW, and OpenGL
if (DEFINED ENV{GLFWDIR})
    list(APPEND CMAKE_PREFIX_PATH $ENV{GLFWDIR})
endif()

find_package(glfw3 3.4 REQUIRED)
target_link_libraries(${TARGET} PRIVATE glfw)	

if (DEFINED ENV{GLEWDIR})
    list(APPEND CMAKE_PREFIX_PATH $ENV{GLEWDIR})

    # windows needs dll in a standard location to find it
    if (WIN32)
        # $<TARGET_RUNTIME_DLLS:GLapp> should work, but is blank for me
        file(GLOB GLEW_LIBS "$ENV{GLEWDIR}/bin/*.dll")
        add_custom_command(
            TARGET ${TARGET} 
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${GLEW_LIBS}
                $<TARGET_FILE_DIR:${TARGET}>
            COMMENT "Copy DLLs"
            COMMAND_EXPAND_LISTS)
    endif()
endif()
find_package(GLEW 2.3.0 REQUIRED)
target_link_libraries(${TARGET} PRIVATE GLEW::glew)

find_package(OpenGL REQUIRED)
target_link_libraries(${TARGET} PRIVATE OpenGL::GL)
