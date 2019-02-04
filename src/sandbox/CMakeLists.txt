project (sandbox)

include(${CMAKE_FRAMEWORK_DIR}/project/project.cmake)

# --------------------- Source -----------------------

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

set (SOURCEFILES
  SceneNode.cpp
  base/TransformComponent.cpp
)

set (HEADERFILES
  Context.h
  SceneContext.h
  SceneNode.h
  SceneComponent.h
  base/TransformComponent.h
)

# --------------------- Executable -----------------------

addStandardLibrary()

