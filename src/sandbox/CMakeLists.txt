project (sandbox)

include(${CMAKE_FRAMEWORK_DIR}/project/project.cmake)

# --------------------- Source -----------------------

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

set (SOURCEFILES
  SceneNode.cpp
  base/Camera.cpp
  base/Image.cpp
  base/Transform.cpp
  data/CSVLoader.cpp
  data/FloatDataSet.cpp
  geometry/Mesh.cpp
  geometry/Material.cpp
  geometry/MeshLoader.cpp
  geometry/shapes/Quad.cpp
  graphics/Blend.cpp
  graphics/FloatDataRenderer.cpp
  graphics/MeshRenderer.cpp
  graphics/ShaderProgram.cpp
  graphics/Texture.cpp
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
  base/Image.h
  base/Transform.h
  data/CSVLoader.h
  data/FloatDataSet.h
  geometry/Mesh.h
  geometry/Material.h
  geometry/MeshLoader.h
  geometry/shapes/Quad.h
  graphics/Blend.h
  graphics/FloatDataRenderer.h
  graphics/MeshRenderer.h
  graphics/ShaderProgram.h
  graphics/Texture.h
  graphics/shaders/MaterialShader.h
  graphics/shaders/PointShader.h
  graphics/shaders/Shader2D.h
)

# --------------------- Dependencies -----------------------

add_external(glm 
	GIT_REPOSITORY https://github.com/g-truc/glm.git
)
add_definitions(-DGLM_FORCE_INLINE)

add_external(stb
  GIT_REPOSITORY git@github.com:nothings/stb.git
  HEADER_ONLY
)

include_directories(
  ${external_dir}/stb/src
)

include(${external_dir}/Assimp/Assimp.cmake)
useAssimp()

include(${external_dir}/OpenGL/OpenGL.cmake)
useOpenGL()

# --------------------- Executable -----------------------

addStandardLibrary()

