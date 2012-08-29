
SET (configuration_SRC
# add here both your sources (cpp) and header (hpp) files
CNCMoveIterator.cpp
CNCMoveIterator.hpp
CommandLineParser.cpp
CommandLineParser.hpp
ConfigFileParser.cpp
ConfigFileParser.hpp
CutterDescription.cpp
CutterDescription.hpp
Geometry.hpp
StockDescription.cpp
StockDescription.hpp
)

ADD_LIBRARY(configuration STATIC ${configuration_SRC})
TARGET_LINK_LIBRARIES(configuration common ${MY_LIBS})


