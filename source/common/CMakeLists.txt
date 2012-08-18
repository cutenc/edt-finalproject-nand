
SET (common_SRC
# add here both your sources (cpp) and header (hpp) files
Color.cpp
Color.hpp
constants.hpp
EulerAngles.cpp
EulerAngles.hpp
Mesh.cpp
Mesh.hpp
Model3D.hpp
Point3D.cpp
Point3D.hpp
Rototraslation.cpp
Rototraslation.hpp
Utilities.cpp
Utilities.hpp
)

ADD_LIBRARY(common STATIC ${common_SRC})
TARGET_LINK_LIBRARIES(common ${MY_LIBS})


