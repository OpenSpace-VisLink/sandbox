project (Planets)

include(${CMAKE_FRAMEWORK_DIR}/project/project.cmake)

# --------------------- Source -----------------------

set (SOURCEFILES
  src/main.cpp
)

set (HEADERFILES
)

# --------------------- Dependencies -----------------------

add_external(tuio 
	GIT_REPOSITORY https://github.com/dtorban/TUIO11_CPP.git
	LIB_NAME TUIO
	DEBUG_POSTFIX d
)

add_external(glm 
	GIT_REPOSITORY https://github.com/g-truc/glm.git
  HEADER_ONLY
)
add_definitions(-DGLM_FORCE_INLINE)
include_directories(${external_dir}/glm/src)

add_external(stb
  GIT_REPOSITORY https://github.com/nothings/stb.git
  HEADER_ONLY
)
include_directories(${external_dir}/stb/src)

find_package(Vulkan REQUIRED)
message("vk Found" ${Vulkan_FOUND} ${Vulkan_INCLUDE_DIRS} ${Vulkan_LIBRARIES})
include_directories(${Vulkan_INCLUDE_DIRS})
set(DEP_LIBS ${DEP_LIBS} ${Vulkan_LIBRARIES})

#find_package(glfw3 REQUIRED)
#set(DEP_LIBS ${DEP_LIBS} glfw)
link_directories(${CMAKE_FRAMEWORK_DIR}/external/glfw3/build/install/lib)
include_directories(${CMAKE_FRAMEWORK_DIR}/external/glfw3/build/install/include)

set(PROJECT_DEPS ${PROJECT_DEPS} sandbox)
set(DEP_LIBS ${DEP_LIBS} sandbox)

include(${external_dir}/OpenGL/OpenGL.cmake)
useOpenGL()

#/home/dan/lib/1.1.114.0/x86_64/bin/glslc examples/VulkanSandbox/src/shaders/shader.vert -o examples/VulkanSandbox/src/shaders/vert.spv
#/home/dan/lib/1.1.114.0/x86_64/bin/glslc examples/VulkanSandbox/src/shaders/shader.frag -o examples/VulkanSandbox/src/shaders/frag.spv
#/home/dan/lib/1.1.114.0/x86_64/bin/glslc examples/VulkanSandbox/src/shaders/shader2.vert -o examples/VulkanSandbox/src/shaders/vert2.spv
#/home/dan/lib/1.1.114.0/x86_64/bin/glslc examples/VulkanSandbox/src/shaders/shader2.frag -o examples/VulkanSandbox/src/shaders/frag2.spv

# --------------------- Executable -----------------------

addStandardExecutable()

