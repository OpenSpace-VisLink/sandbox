project (sandbox)

include(${CMAKE_FRAMEWORK_DIR}/project/project.cmake)

# --------------------- Source -----------------------

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

set (SOURCEFILES
  Entity.cpp
  base/Transform.cpp
  graphics/GraphicsContextRenderer.cpp
)

set (HEADERFILES
  Component.h
  Context.h
  Entity.h
  StateContainer.h
  Context.h
  base/Transform.h
  graphics/GraphicsComponent.h
  graphics/GraphicsContext.h
  graphics/GraphicsContextRenderer.h
  graphics/RenderCallback.h
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

