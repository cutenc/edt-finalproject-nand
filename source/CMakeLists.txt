PROJECT(edt-finalProject CXX)

#
# INITIALIZATION VARIABLE
#
CMAKE_MINIMUM_REQUIRED(VERSION 2.6 FATAL_ERROR)

# help Eclipse gcc error parsing disabling multi-line behaviour
IF(CMAKE_COMPILER_IS_GNUCC)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCC)
IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

# help Eclipse includes discovery from Makefile '-l' argument
SET(CMAKE_VERBOSE_MAKEFILE ON)

# needed in order to use gdb within Eclipse
SET(CMAKE_BUILD_TYPE Debug)

# Compiler Options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")


##############

#
# SOME PROJECT SPECIFIC LIBRARIES
#
FIND_LIBRARY(BOOST REQUIRED)
INCLUDE_DIRECTORIES(${BOOST_INCLUDE_DIRS})
LINK_DIRECTORIES(${BOOST_LIBRARY_DIRS})
ADD_DEFINITIONS(${BOOST_DEFINITIONS})    
SET (MY_LIBS ${MY_LIBS} ${BOOST_LIBRARIES})

###############

#
# PROJECT SECTION
#
# include current directory in the inclusion directive
SET(CMAKE_INCLUDE_CURRENT_DIR TRUE)

SET(MY_SRCS
main.cpp)

# now include other directories in order to append other files
SET(MY_FOLDERS
common
configuration
meshing
milling
visualizer
)
FOREACH(dir ${MY_FOLDERS})
    ADD_SUBDIRECTORY(${dir})
ENDFOREACH(dir)

ADD_EXECUTABLE(CNCSimulator ${MY_SRCS})
TARGET_LINK_LIBRARIES(CNCSimulator ${MY_FOLDERS} ${MY_LIBS})
