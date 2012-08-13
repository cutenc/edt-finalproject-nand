
SET (milling_SRC
# add here both your sources (cpp) and header (hpp) files

)

ADD_LIBRARY(milling STATIC ${milling_SRC})
TARGET_LINK_LIBRARIES(milling common ${MY_LIBS})


