PROJECT(edt-finalProject CXX)

#
# INITIALIZATION VARIABLE
#
CMAKE_MINIMUM_REQUIRED(VERSION 2.6 FATAL_ERROR)

# help Eclipse gcc error parsing disabling multi-line behaviour
IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()

# help Eclipse includes discovery from Makefile '-l' argument
SET(CMAKE_VERBOSE_MAKEFILE ON)

# Compiler Options in debug mode
IF (${WIN32})
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -ggdb -O0")
ELSEIF(${UNIX})
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -ggdb -O0")
ELSE()
    MESSAGE(WARNING "** Arch not supported **")
ENDIF()

##############

#
# SOME PROJECT SPECIFIC LIBRARIES
#

# BOOST libraries
SET(Boost_USE_MULTITHREADED         ON)
SET(Boost_USE_STATIC_LIBS       	OFF) #mettere a on questo se link statico
SET(Boost_USE_STATIC_RUNTIME    	OFF)
SET(Boost_MIN_VERSION               "1.48.0")

FIND_PACKAGE(Boost REQUIRED COMPONENTS regex program_options chrono system signals thread)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
ADD_DEFINITIONS(${Boost_DEFINITIONS})
SET (MY_LIBS ${MY_LIBS} ${Boost_LIBRARIES})

# commentare le seguenti se build statico (ci sono le librerie statiche?)
ADD_DEFINITIONS(-DBOOST_ALL_NO_LIB)
#ADD_DEFINITIONS(-DBOOST_ALL_DYN_LINK)
ADD_DEFINITIONS(-DBOOST_PROGRAM_OPTIONS_DYN_LINK)



# OpenSceneGraph
FIND_PACKAGE( OpenSceneGraph 3.0.0 REQUIRED osgUtil osgText osgViewer osgGA )
INCLUDE_DIRECTORIES(${OPENSCENEGRAPH_INCLUDE_DIRS})
SET (MY_LIBS ${MY_LIBS} ${OPENSCENEGRAPH_LIBRARIES})

# EIGEN
IF (${WIN32})
    INCLUDE_DIRECTORIES("$ENV{EIGEN_ROOT}/include")
ELSEIF(${UNIX})
    INCLUDE_DIRECTORIES("/usr/include/eigen3")
ELSE()
    MESSAGE(WARNING "** Arch not supported **")
ENDIF()


# output MY_LIBS definition
MESSAGE(STATUS "*** MY_LIBS: ${MY_LIBS}")
 
###############

#
# PROJECT SECTION
#
# include current directory in the inclusion directive
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
