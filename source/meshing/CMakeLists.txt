
SET (meshing_SRC
# add here both your sources (cpp) and header (hpp) files
Mesher.hpp
StockMesher.cpp
StockMesher.hpp
)

ADD_LIBRARY(meshing STATIC ${meshing_SRC})
TARGET_LINK_LIBRARIES(meshing common ${MY_LIBS})


