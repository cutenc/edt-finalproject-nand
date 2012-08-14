
SET (configuration_SRC
# add here both your sources (cpp) and header (hpp) files
CNCMoveIterator.cpp
CNCMoveIterator.hpp
ConfigurationManager.cpp
ConfigurationManager.hpp
CutterDescription.cpp
CutterDescription.hpp
Geometry.hpp
StockDescriptor.cpp
StockDescriptor.hpp
)

ADD_LIBRARY(configuration STATIC ${configuration_SRC})
TARGET_LINK_LIBRARIES(configuration common ${MY_LIBS})


