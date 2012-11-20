
SET (meshing_SRC
# add here both your sources (cpp) and header (hpp) files
BranchNodeData.hpp
CommonMesher.cpp
CommonMesher.hpp
Face.cpp
Face.hpp
leaf_node_callbacks.hpp
mesherCallbacks/BoxMesherCallback.hpp
mesherCallbacks/MarchingCubeConstants.hpp
mesherCallbacks/MarchingCubeMesherCallback.cpp
mesherCallbacks/MarchingCubeMesherCallback.hpp
mesherCallbacks/MeshingVoxel.cpp
mesherCallbacks/MeshingVoxel.hpp
LeafNodeCallback.hpp
LeafNodeData.cpp
LeafNodeData.hpp
MarchingCubeMesher.hpp
Mesher.hpp
MeshingUtils.cpp
MeshingUtils.hpp
MeshOctree.cpp
MeshOctree.hpp
OctreeNodeData.cpp
OctreeNodeData.hpp
StubMesher.hpp
VoxelMesher.hpp
)

ADD_LIBRARY(meshing STATIC ${meshing_SRC})
TARGET_LINK_LIBRARIES(meshing common ${MY_LIBS})


