project (sandbox)

include(${CMAKE_FRAMEWORK_DIR}/project/project.cmake)

# --------------------- Source -----------------------

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

set (SOURCEFILES
  Entity.cpp
  base/Transform.cpp
  geometry/Material.cpp
  geometry/Mesh.cpp
  geometry/MeshLoader.cpp
  geometry/loaders/QuadLoader.cpp
  geometry/loaders/ShapeLoader.cpp
  #graphics/GraphicsContextRenderer.cpp
  #graphics/render/FrameBuffer.cpp
  #graphics/render/MeshRenderer.cpp
  #graphics/render/ShaderProgram.cpp
  #graphics/render/Texture.cpp
  #graphics/render/effects/Blend.cpp
  #graphics/render/effects/CullFace.cpp
  #graphics/render/shaders/BasicShader.cpp
  #graphics/render/shaders/MaterialShader.cpp
  #graphics/render/shaders/ShaderParameterTypes.cpp
  #graphics/view/Camera.cpp
  image/Image.cpp
  input/interaction/ArcBall.cpp
  input/interaction/MouseZoom.cpp
  input/interaction/MouseTranslate.cpp
  input/interaction/VirtualCursor.cpp
  io/File.cpp
  io/FileMonitor.cpp
  graphics/vulkan/VulkanDevice.cpp
  graphics/vulkan/render/VulkanRenderPass.cpp
  graphics/vulkan/render/VulkanCommandPool.cpp
  graphics/vulkan/render/VulkanCommandBuffer.cpp
  graphics/vulkan/render/VulkanGraphicsPipeline.cpp
  graphics/vulkan/shader/VulkanDescriptor.cpp
  graphics/vulkan/shader/VulkanShaderModule.cpp
  graphics/vulkan/VulkanDeviceRenderer.cpp
  graphics/vulkan/VulkanSwapChain.cpp
)

set (HEADERFILES
  Component.h
  Context.h
  Entity.h
  StateContainer.h
  Context.h
  base/Transform.h
  geometry/Material.h
  geometry/Mesh.h
  geometry/MeshLoader.h
  geometry/loaders/QuadLoader.h
  geometry/loaders/ShapeLoader.h
  graphics/GraphicsComponent.h
  graphics/GraphicsContext.h
  graphics/GraphicsContextRenderer.h
  graphics/RenderCallback.h
  graphics/render/FrameBuffer.h
  graphics/render/MeshRenderer.h
  graphics/render/ShaderProgram.h
  graphics/render/Texture.h
  graphics/render/effects/Blend.h
  graphics/render/effects/CullFace.h
  graphics/render/shaders/BasicShader.h
  graphics/render/shaders/MaterialShader.h
  graphics/render/shaders/ShaderParameterTypes.h
  graphics/view/Camera.h
  image/Image.h
  input/MouseInput.h
  input/interaction/ArcBall.h
  input/interaction/MouseZoom.h
  input/interaction/MouseTranslate.h
  input/interaction/VirtualCursor.h
  io/File.h
  io/FileMonitor.h
)

# --------------------- Dependencies -----------------------

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  find_package(glfw3 REQUIRED)
  set(DEP_LIBS ${DEP_LIBS} glfw)
else()
  add_external(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    CMAKE_ARGS glfwArgs
    LIB_NAME glfw3
  )
  include_directories(${CMAKE_FRAMEWORK_DIR}/external/glfw/build/install/include)
endif()

find_package(Vulkan REQUIRED)
message("vk Found" ${Vulkan_FOUND} ${Vulkan_INCLUDE_DIRS} ${Vulkan_LIBRARIES})
include_directories(${Vulkan_INCLUDE_DIRS})
set(DEP_LIBS ${DEP_LIBS} ${Vulkan_LIBRARIES})

add_external(glm 
	GIT_REPOSITORY https://github.com/g-truc/glm.git
	HEADER_ONLY
)
add_definitions(-DGLM_FORCE_INLINE)
include_directories(${CMAKE_FRAMEWORK_DIR}/external/glm/src)

add_external(stb
  GIT_REPOSITORY https://github.com/nothings/stb.git
  HEADER_ONLY
)

include_directories(
  ${external_dir}/stb/src
)

#include(${external_dir}/Assimp/Assimp.cmake)
#useAssimp()

include(${external_dir}/OpenGL/OpenGL.cmake)
useOpenGL()
#useGLEW()
#useGlad()

# --------------------- Executable -----------------------

addStandardLibrary()

