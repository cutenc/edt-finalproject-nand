
SET (milling_SRC
# add here both your sources (cpp) and header (hpp) files
Corner.hpp
Cutter.cpp
Cutter.hpp
cutters.hpp
MeshingInfo.cpp
MeshingInfo.hpp
MillingAlgorithm.cpp
MillingAlgorithm.hpp
MillingResult.hpp
octree_nodes.hpp
Octree.hpp
ShiftedBox.hpp
SimpleBox.hpp
Stock.cpp
Stock.hpp
Voxel.cpp
Voxel.hpp
VoxelInfo.hpp
)

ADD_LIBRARY(milling STATIC ${milling_SRC})
TARGET_LINK_LIBRARIES(milling common ${MY_LIBS})


