PROJECT(edt-finalProject CXX)

#
# INITIALIZATION VARIABLE
#
CMAKE_MINIMUM_REQUIRED(VERSION 2.6 FATAL_ERROR)

# help Eclipse gcc error parsing disabling multi-line behaviour
IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

# help Eclipse includes discovery from Makefile '-l' argument
SET(CMAKE_VERBOSE_MAKEFILE ON)

# Compiler Options
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -ggdb -O0 -losg -losgViewer -losgGA")


##############

#
# SOME PROJECT SPECIFIC LIBRARIES
#

# BOOST

SET(BOOST_MIN_VERSION "1.48.0")

FIND_PACKAGE(Boost COMPONENTS regex program_options chrono system signals thread REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
ADD_DEFINITIONS(${Boost_DEFINITIONS})
SET (MY_LIBS ${MY_LIBS} ${Boost_LIBRARIES})

FIND_PACKAGE( OpenSceneGraph 3.0.0 REQUIRED osgUtil osgText osgViewer osgGA )
INCLUDE_DIRECTORIES(${OPENSCENEGRAPH_INCLUDE_DIRS})
SET (MY_LIBS ${MY_LIBS} ${OPENSCENEGRAPH_LIBRARIES})

# EIGEN
INCLUDE_DIRECTORIES("/usr/include/eigen3")

# OSG
INCLUDE_DIRECTORIES("usr/include/osg")
INCLUDE_DIRECTORIES("usr/include/osgGA")
INCLUDE_DIRECTORIES("usr/include/osgManipulator")
INCLUDE_DIRECTORIES("usr/include/osgViewer")

# MATH LIBRARY
SET (MY_LIBS ${MY_LIBS} m)

# output MY_LIBS definition
MESSAGE(STATUS "*** MY_LIBS: ${MY_LIBS}")
 
###############

#
# PROJECT SECTION
#
# include current directory in the inclusion directive
#SET(CMAKE_INCLUDE_CURRENT_DIR TRUE)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})

SET(MY_SRCS
main.cpp)

# now include other directories in order to append other files
SET(MY_FOLDERS
common
configuration
meshing
milling
threading
visualizer
)
FOREACH(dir ${MY_FOLDERS})
    ADD_SUBDIRECTORY(${dir})
ENDFOREACH(dir)

ADD_EXECUTABLE(CNCSimulator ${MY_SRCS})
TARGET_LINK_LIBRARIES(CNCSimulator ${MY_FOLDERS} ${MY_LIBS})
