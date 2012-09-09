
SET (visualizer_SRC
# add here both your sources (cpp) and header (hpp) files
CyclicRunnable.cpp
CyclicRunnable.hpp
MillerRunnable.cpp
MillerRunnable.hpp
MillingSignaler.cpp
MillingSignaler.hpp
Runnable.hpp
SignaledInfo.hpp
SteppableController.cpp
SteppableController.hpp
SteppableRunnable.cpp
SteppableRunnable.hpp
)

ADD_LIBRARY(visualizer STATIC ${visualizer_SRC})
TARGET_LINK_LIBRARIES(visualizer common ${MY_LIBS})


