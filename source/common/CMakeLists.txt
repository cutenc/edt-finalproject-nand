
SET (common_SRC
Point3D.cpp
Point3D.hpp
)

ADD_LIBRARY(common STATIC ${common_SRC})
TARGET_LINK_LIBRARIES(common ${MY_LIBS})


