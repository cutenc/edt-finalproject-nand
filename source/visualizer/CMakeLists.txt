
SET (visualizer_SRC
# add here both your sources (cpp) and header (hpp) files
Display.cpp
Display.hpp
DisplayInfo.hpp
DisplayTextual.cpp
DisplayTextual.hpp
InputDeviceStateType.cpp
InputDeviceStateType.hpp
KeyboardHandler.cpp
KeyboardHandler.hpp
KeyboardManager.cpp
KeyboardManager.hpp
MillerRunnable.cpp
MillerRunnable.hpp
MillingSignaler.cpp
MillingSignaler.hpp
SceneUpdater.cpp
SceneUpdater.hpp
SignaledInfo.hpp
VisualizationUtils.cpp
VisualizationUtils.hpp
)

ADD_LIBRARY(visualizer STATIC ${visualizer_SRC})
TARGET_LINK_LIBRARIES(visualizer common ${MY_LIBS})


