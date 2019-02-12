project (sandbox)

include(${CMAKE_FRAMEWORK_DIR}/project/project.cmake)

# --------------------- Source -----------------------

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

set (SOURCEFILES
  SceneNode.cpp
  base/Camera.cpp
  base/Transform.cpp
  data/FloatDataSet.cpp
  geometry/Mesh.cpp
  geometry/Material.cpp
  geometry/MeshLoader.cpp
  geometry/shapes/Quad.cpp
  graphics/FloatDataRenderer.cpp
  graphics/MeshRenderer.cpp
  graphics/ShaderProgram.cpp
  graphics/shaders/MaterialShader.cpp
  graphics/shaders/PointShader.cpp
  graphics/shaders/Shader2D.cpp
)

set (HEADERFILES
  Context.h
  SceneContext.h
  SceneNode.h
  SceneComponent.h
  base/Camera.h
  base/Transform.h
  data/FloatDataSet.h
  geometry/Mesh.h
  geometry/Material.h
  geometry/MeshLoader.h
  geometry/shapes/Quad.h
  graphics/FloatDataRenderer.h
  graphics/MeshRenderer.h
  graphics/ShaderProgram.h
  graphics/shaders/MaterialShader.h
  graphics/shaders/PointShader.cpp
  graphics/shaders/Shader2D.h
)

# --------------------- Dependencies -----------------------

add_external(glm 
	GIT_REPOSITORY https://github.com/g-truc/glm.git
)
add_definitions(-DGLM_FORCE_INLINE)

include(${external_dir}/Assimp/Assimp.cmake)
useAssimp()

include(${external_dir}/OpenGL/OpenGL.cmake)
useOpenGL()

# --------------------- Executable -----------------------

addStandardLibrary()

