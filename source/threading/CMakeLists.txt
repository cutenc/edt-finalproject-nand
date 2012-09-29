
SET (common_SRC
# add here both your sources (cpp) and header (hpp) files
CyclicRunnable.cpp
CyclicRunnable.hpp
Runnable.hpp
SteppableController.cpp
SteppableController.hpp
SteppableRunnable.cpp
SteppableRunnable.hpp
)

ADD_LIBRARY(threading STATIC ${common_SRC})
TARGET_LINK_LIBRARIES(threading ${MY_LIBS})


