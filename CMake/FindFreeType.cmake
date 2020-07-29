message(STATUS "Looking for FreeType")

set(FREETYPE_SEARCH_PATHS 
    ${FREETYPE_DIR} 
    ${PROJECT_SOURCE_DIR}/ThirdParty/FreeType 
    /usr/local 
    /opt/local 
    /usr 
    /opt 
)

# for some reason Anaconda's FreeType showed up before the project's 
list(APPEND FREETYPE_SEARCH_PATHS $ENV{PATH})

# find include directory 
find_path(FREETYPE_INCLUDE_DIR 
    NAMES ft2build.h 
    PATHS ${FREETYPE_SEARCH_PATHS} 
    PATH_SUFFIXES include include/freetype2
    NO_PACKAGE_ROOT_PATH
    NO_SYSTEM_ENVIRONMENT_PATH
    NO_SYSTEM_PATH
    NO_CMAKE_PATH
    NO_CMAKE_ENVIRONMENT_PATH
    NO_DEFAULT_PATH 
)

# find FreeType library 
find_library(FREETYPE_FREETYPE
    NAMES freetype
    PATHS ${FREETYPE_SEARCH_PATHS} 
    PATH_SUFFIXES objs lib lib64
)

# finish finding FreeType 
if(FREETYPE_INCLUDE_DIR AND FREETYPE_FREETYPE) 
    set(FREETYPE_INCLUDE_DIRS ${FREETYPE_INCLUDE_DIR})
    set(FREETYPE_LIBRARIES ${FREETYPE_FREETYPE})

    set(FREETYPE_FOUND true)
    message(STATUS "Looking for FreeType - found")
    message(STATUS "FreeType include dir: ${FREETYPE_INCLUDE_DIRS}")
    message(STATUS "FreeType libraries: ${FREETYPE_LIBRARIES}")
else() 
    # set(FREETYPE_FOUND FALSE)
    message(STATUS "Looking for FreeType - not found")
endif() 

if(FREETYPE_FIND_REQUIRED AND NOT FREETYPE_FOUND) 
    message(FATAL_ERROR "Could not find FreeType")
endif()
