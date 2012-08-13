
SET (configuration_SRC
# add here both your sources (cpp) and header (hpp) files

)

ADD_LIBRARY(configuration STATIC ${configuration_SRC})
TARGET_LINK_LIBRARIES(configuration common ${MY_LIBS})


