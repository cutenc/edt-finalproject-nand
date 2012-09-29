
SET (visualizer_SRC
# add here both your sources (cpp) and header (hpp) files
MesherStub.hpp
MillerEndedException.hpp
MillerRunnable.cpp
MillerRunnable.hpp
MillingSignaler.cpp
MillingSignaler.hpp
SignaledInfo.hpp
)

ADD_LIBRARY(visualizer STATIC ${visualizer_SRC})
TARGET_LINK_LIBRARIES(visualizer common ${MY_LIBS})


