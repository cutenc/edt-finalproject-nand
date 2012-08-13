
SET (visualizer_SRC
# add here both your sources (cpp) and header (hpp) files

)

ADD_LIBRARY(visualizer STATIC ${visualizer_SRC})
TARGET_LINK_LIBRARIES(visualizer common ${MY_LIBS})


