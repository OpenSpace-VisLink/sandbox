project (VulkanSandbox)

include(${CMAKE_FRAMEWORK_DIR}/project/project.cmake)

# --------------------- Source -----------------------

set (SOURCEFILES
  src/main.cpp
)

set (HEADERFILES
)

# --------------------- Dependencies -----------------------

add_definitions(-DGLM_FORCE_INLINE)

include(${external_dir}/NanoGUI/NanoGUI.cmake)
#useNanoGUI()

include(${external_dir}/CPP11/CPP11.cmake)
useCPP17()

include(${external_dir}/OpenGL/OpenGL.cmake)
#useOpenGL()

find_package(Vulkan REQUIRED)
message("vk Found" ${Vulkan_FOUND} ${Vulkan_INCLUDE_DIRS} ${Vulkan_LIBRARIES})
include_directories(${Vulkan_INCLUDE_DIRS})
set(DEP_LIBS ${DEP_LIBS} ${Vulkan_LIBRARIES})

find_package(glfw3 REQUIRED)
set(DEP_LIBS ${DEP_LIBS} glfw)

set(PROJECT_DEPS ${PROJECT_DEPS} sandbox)
set(DEP_LIBS ${DEP_LIBS} sandbox)

add_external(stb
  GIT_REPOSITORY git@github.com:nothings/stb.git
  HEADER_ONLY
)

include_directories(
  ${external_dir}/stb/src
)


#/home/dan/lib/1.1.114.0/x86_64/bin/glslc examples/VulkanExample/src/shaders/shader.frag -o examples/VulkanExample/src/shaders/frag.spv

# --------------------- Executable -----------------------

addStandardExecutable()

