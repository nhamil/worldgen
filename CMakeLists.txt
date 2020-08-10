cmake_minimum_required(VERSION 2.8) 
project(WorldGen CXX) 

set(WORLDGEN_APP_NAME WorldGen) 
set(WORLDGEN_SOURCE_FOLDER Source/WorldGen) 
set(FJORD_SOURCE_FOLDER Source/Fjord) 

# includes 
include_directories(Source)

# source files 
set(SOURCES 
    ${WORLDGEN_SOURCE_FOLDER}/main.cpp 
    ${WORLDGEN_SOURCE_FOLDER}/World.cpp 
    ${WORLDGEN_SOURCE_FOLDER}/WorldGenerator.cpp 
    ${WORLDGEN_SOURCE_FOLDER}/WorldGenSystem.cpp 

    ${WORLDGEN_SOURCE_FOLDER}/ClimateSimulationRule.cpp 
    ${WORLDGEN_SOURCE_FOLDER}/BasicTerrainGenRule.cpp 
    ${WORLDGEN_SOURCE_FOLDER}/CellDistortRule.cpp 
    ${WORLDGEN_SOURCE_FOLDER}/CellRelaxRule.cpp 
    ${WORLDGEN_SOURCE_FOLDER}/SubdivideCellGenRule.cpp 
    
    ${FJORD_SOURCE_FOLDER}/Core/Application.cpp 
    ${FJORD_SOURCE_FOLDER}/Core/Engine.cpp 
    ${FJORD_SOURCE_FOLDER}/Core/Log.cpp 
    ${FJORD_SOURCE_FOLDER}/Core/UI.cpp 
    ${FJORD_SOURCE_FOLDER}/Core/Window.cpp 

    ${FJORD_SOURCE_FOLDER}/Graphics/Color.cpp 
    ${FJORD_SOURCE_FOLDER}/Graphics/Font.cpp 
    ${FJORD_SOURCE_FOLDER}/Graphics/IndexBuffer.cpp 
    ${FJORD_SOURCE_FOLDER}/Graphics/Geometry.cpp 
    ${FJORD_SOURCE_FOLDER}/Graphics/Graphics.cpp 
    ${FJORD_SOURCE_FOLDER}/Graphics/Light.cpp 
    ${FJORD_SOURCE_FOLDER}/Graphics/Material.cpp
    ${FJORD_SOURCE_FOLDER}/Graphics/Mesh.cpp 
    ${FJORD_SOURCE_FOLDER}/Graphics/OpenGL.cpp 
    ${FJORD_SOURCE_FOLDER}/Graphics/Parameter.cpp 
    ${FJORD_SOURCE_FOLDER}/Graphics/Renderer.cpp 
    ${FJORD_SOURCE_FOLDER}/Graphics/RenderSystems.cpp 
    ${FJORD_SOURCE_FOLDER}/Graphics/Shader.cpp 
    ${FJORD_SOURCE_FOLDER}/Graphics/SpriteBatch.cpp 
    ${FJORD_SOURCE_FOLDER}/Graphics/Texture2D.cpp 
    ${FJORD_SOURCE_FOLDER}/Graphics/VertexBuffer.cpp 

    ${FJORD_SOURCE_FOLDER}/Math/Math.cpp 

    ${FJORD_SOURCE_FOLDER}/Scene/EntitySystem.cpp 
    ${FJORD_SOURCE_FOLDER}/Scene/Group.cpp 
    ${FJORD_SOURCE_FOLDER}/Scene/Scene.cpp 

    ${FJORD_SOURCE_FOLDER}/Util/FileUtil.cpp 
    ${FJORD_SOURCE_FOLDER}/Util/Memory.cpp 
    ${FJORD_SOURCE_FOLDER}/Util/Random.cpp 
    ${FJORD_SOURCE_FOLDER}/Util/Thread.cpp 
    ${FJORD_SOURCE_FOLDER}/Util/Time.cpp 
)

# platform specific sources 
if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()
if(LINUX) 
    message(STATUS "Building for Linux") 
elseif(WIN32) 
    message(STATUS "Building for Windows") 
else() 
    message(FATAL_ERROR "Unsupported operating system") 
endif() 

# set up build type 
if(NOT CMAKE_BUILD_TYPE) 
    message(STATUS "Build type automatically set to DEBUG")
    set(CMAKE_BUILD_TYPE DEBUG)
else() 
    message(STATUS "Build type set to ${CMAKE_BUILD_TYPE}")
endif() 

# check bit mode 
if(CMAKE_SIZEOF_VOID_P EQUAL 4) 
    message(STATUS "Building 32-bit")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8) 
    message(STATUS "Building 64-bit")
else() 
    math(EXPR BIT_COUNT "${CMAKE_SIZEOF_VOID_P} * 8")
    message(STATUS "Unknown bit mode: " ${BIT_COUNT})
endif() 

# compilation flags 
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -pedantic -g -std=c++0x -fmax-errors=3 -static -static-libgcc -static-libstdc++")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O3 -std=c++0x -static -static-libgcc -static-libstdc++")

# add Find<Lib>.cmake files to path 
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/")

# find OpenGL
find_package(OpenGL REQUIRED)

# find SDL2 
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS}) 

# find GLEW 
find_package(GLEW REQUIRED) 
include_directories(${GLEW_INCLUDE_DIRS})

# find FreeType
find_package(FreeType REQUIRED) 
include_directories(${FREETYPE_INCLUDE_DIRS})

# build project 
add_executable(${WORLDGEN_APP_NAME} ${SOURCES}) 
target_link_libraries(${WORLDGEN_APP_NAME} 
    ${SDL2_LIBRARIES}
    ${GLEW_LIBRARIES} 
    ${OPENGL_LIBRARIES} 
    ${FREETYPE_LIBRARIES}
)
