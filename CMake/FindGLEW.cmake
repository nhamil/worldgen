message(STATUS "Looking for GLEW")

set(GLEW_SEARCH_PATHS 
    ${GLEW_DIR} 
    ${PROJECT_SOURCE_DIR}/ThirdParty/GLEW 
    /usr/local 
    /opt/local 
    /usr 
    /opt 
)

# find include directory 
find_path(GLEW_INCLUDE_DIR 
    NAMES GL/glew.h 
    PATHS ${GLEW_SEARCH_PATHS} 
    PATH_SUFFIXES include 
)

# find glew32 library 
find_library(GLEW_GLEW32
    NAMES glew32.dll GLEW glew32 
    PATHS ${GLEW_SEARCH_PATHS} 
    PATH_SUFFIXES lib lib64
)

# finish finding GLEW 
if(GLEW_INCLUDE_DIR AND GLEW_GLEW32) 
    set(GLEW_INCLUDE_DIRS ${GLEW_INCLUDE_DIR})
    set(GLEW_LIBRARIES ${GLEW_GLEW32})

    set(GLEW_FOUND true)
    message(STATUS "Looking for GLEW - found")
    message(STATUS "GLEW include dir: ${GLEW_INCLUDE_DIRS}")
    message(STATUS "GLEW libraries: ${GLEW_LIBRARIES}")
else() 
    #set(GLEW_FOUND FALSE)
    message(STATUS "Looking for GLEW - not found")
endif() 

if(GLEW_FIND_REQUIRED AND NOT GLEW_FOUND) 
    message(FATAL_ERROR "Could not find GLEW")
endif()
