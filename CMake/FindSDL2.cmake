message(STATUS "Looking for SDL2")

set(SDL2_SEARCH_PATHS 
    ${SDL2_DIR} 
    ${PROJECT_SOURCE_DIR}/ThirdParty/SDL2 
    /usr/local 
    /opt/local 
    /usr 
    /opt 
)

# MinGW folders are set up differently from VS 
if (MINGW) 
    if(CMAKE_SIZEOF_VOID_P EQUAL 8) 
        list(APPEND SDL2_SEARCH_PATHS ${PROJECT_SOURCE_DIR}/ThirdParty/SDL2/x86_64-w64-mingw32/)
    else() 
        list(APPEND SDL2_SEARCH_PATHS ${PROJECT_SOURCE_DIR}/ThirdParty/SDL2/i686-w64-mingw32/)
    endif() 
endif() 

# find include directory 
find_path(SDL2_INCLUDE_DIR 
    NAMES SDL2/SDL.h 
    PATHS ${SDL2_SEARCH_PATHS} 
    PATH_SUFFIXES include 
)

# find SDL2.dll library 
find_library(SDL2_LIB_SDL2DLL
    NAMES SDL2.dll SDL2
    PATHS ${SDL2_SEARCH_PATHS} 
    PATH_SUFFIXES lib lib64
)

# find SDL2main library 
find_library(SDL2_LIB_SDL2MAIN
    NAMES SDL2main
    PATHS ${SDL2_SEARCH_PATHS} 
    PATH_SUFFIXES lib lib64
)

# finish finding SDL2 
if(SDL2_INCLUDE_DIR AND SDL2_LIB_SDL2MAIN AND SDL2_LIB_SDL2DLL) 
    set(SDL2_LIBRARIES ${SDL2_LIB_SDL2MAIN} ${SDL2_LIB_SDL2DLL})
    set(SDL2_INCLUDE_DIRS ${SDL2_INCLUDE_DIR})

    # MinGW needs -lmingw32 before SDL2 libraries 
    if(MINGW) 
        set(SDL2_LIBRARIES mingw32 ${SDL2_LIBRARIES})
        unset(MINGW_LIBRARY)
    endif() 

    set(SDL2_FOUND true)
    message(STATUS "Looking for SDL2 - found")
    message(STATUS "SDL2 include dir: ${SDL2_INCLUDE_DIRS}")
    message(STATUS "SDL2 libraries: ${SDL2_LIBRARIES}")
else() 
    #set(SDL2_FOUND FALSE)
    message(STATUS "Looking for SDL2 - not found")
endif() 

if(SDL2_FIND_REQUIRED AND NOT SDL2_FOUND) 
    message(FATAL_ERROR "Could not find SDL2")
endif()
