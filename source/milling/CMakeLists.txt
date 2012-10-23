
SET (milling_SRC
# add here both your sources (cpp) and header (hpp) files
Adjacencies.cpp
Adjacencies.hpp
Corner.hpp
Cutter.cpp
Cutter.hpp
cutters.hpp
IntersectionResult.cpp
IntersectionResult.hpp
MillingAlgorithm.cpp
MillingAlgorithm.hpp
MillingAlgorithmConf.hpp
MillingResult.hpp
octree_nodes.hpp
Octree.hpp
PtrVersioner.hpp
ShiftedBox.hpp
SimpleBox.hpp
Stock.cpp
Stock.hpp
StoredData.cpp
StoredData.hpp
Voxel.cpp
Voxel.hpp
VoxelInfo.cpp
VoxelInfo.hpp
)

ADD_LIBRARY(milling STATIC ${milling_SRC})
TARGET_LINK_LIBRARIES(milling common ${MY_LIBS})


