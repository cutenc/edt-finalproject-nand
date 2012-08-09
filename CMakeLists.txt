#
# INITIALIZATION VARIABLE
#

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


#
# PROJECT SECTION
#
PROJECT(edt-finalProject)

