project (VulkanSandbox)

include(${CMAKE_FRAMEWORK_DIR}/project/project.cmake)

# --------------------- Source -----------------------

set (SOURCEFILES
  src/main.cpp
)

set (HEADERFILES
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

#find_package(glfw3 REQUIRED)
#set(DEP_LIBS ${DEP_LIBS} glfw)

#add_external(tuio 
#	GIT_REPOSITORY https://github.com/dtorban/TUIO11_CPP.git
#	LIB_NAME TUIO
#)

add_external(glm 
	GIT_REPOSITORY https://github.com/g-truc/glm.git
	HEADER_ONLY
)
add_definitions(-DGLM_FORCE_INLINE)
include_directories(${CMAKE_FRAMEWORK_DIR}/external/glm/src)

include(${external_dir}/NanoGUI/NanoGUI.cmake)
#useNanoGUI()

include(${external_dir}/CPP11/CPP11.cmake)
#useCPP17()

include(${external_dir}/OpenGL/OpenGL.cmake)
#useOpenGL()

find_package(Vulkan REQUIRED)
message("vk Found" ${Vulkan_FOUND} ${Vulkan_INCLUDE_DIRS} ${Vulkan_LIBRARIES})
include_directories(${Vulkan_INCLUDE_DIRS})
set(DEP_LIBS ${DEP_LIBS} ${Vulkan_LIBRARIES})


set(PROJECT_DEPS ${PROJECT_DEPS} sandbox)
set(DEP_LIBS ${DEP_LIBS} sandbox)

add_external(stb
  GIT_REPOSITORY git@github.com:nothings/stb.git
  HEADER_ONLY
)

include_directories(
  ${external_dir}/stb/src
)

include(${external_dir}/OpenGL/OpenGL.cmake)
useOpenGL()
#useGLEW()
#useGlad()


#/home/dan/lib/1.1.114.0/x86_64/bin/glslc examples/VulkanSandbox/src/shaders/shader.vert -o examples/VulkanSandbox/src/shaders/vert.spv
#/home/dan/lib/1.1.114.0/x86_64/bin/glslc examples/VulkanSandbox/src/shaders/shader.frag -o examples/VulkanSandbox/src/shaders/frag.spv
#/home/dan/lib/1.1.114.0/x86_64/bin/glslc examples/VulkanSandbox/src/shaders/shader2.vert -o examples/VulkanSandbox/src/shaders/vert2.spv
#/home/dan/lib/1.1.114.0/x86_64/bin/glslc examples/VulkanSandbox/src/shaders/shader2.frag -o examples/VulkanSandbox/src/shaders/frag2.spv

# --------------------- Executable -----------------------

addStandardExecutable()

