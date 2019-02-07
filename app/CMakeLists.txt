project (ExampleApp)

include(${CMAKE_FRAMEWORK_DIR}/project/project.cmake)

# --------------------- Source -----------------------

set (SOURCEFILES
  src/main.cpp
)

set (HEADERFILES
)

# --------------------- Dependencies -----------------------

add_external(glm 
	GIT_REPOSITORY https://github.com/g-truc/glm.git
)
add_definitions(-DGLM_FORCE_INLINE)

set(NanoGUIArgs -DNANOGUI_BUILD_SHARED=OFF -DNANOGUI_BUILD_PYTHON=OFF -DNANOGUI_BUILD_EXAMPLE=OFF -DCMAKE_C_FLAGS=-fPIC -DCMAKE_CXX_FLAGS=-fPIC  -DCMAKE_DEBUG_POSTFIX=d -DCMAKE_CONFIGURATION_TYPES='Debug Release MinSizeRel RelWithDebInfo')
add_external(NanoGUI 
	GIT_REPOSITORY https://github.com/wjakob/nanogui.git
	CMAKE_ARGS NanoGUIArgs
	LIB_NAME nanogui
)

include_directories(
	${external_dir}/NanoGUI/build/install/include
	${external_dir}/NanoGUI/src/ext/eigen
	${external_dir}/NanoGUI/src/ext/glfw/include
	${external_dir}/NanoGUI/src/ext/nanovg/src
)

include(${external_dir}/OpenGL/OpenGL.cmake)
useOpenGL()

include(${external_dir}/Assimp/Assimp.cmake)
useAssimp()

include(${external_dir}/CPP11/CPP11.cmake)
useCPP11()

include(${external_dir}/OpenGL/OpenGL.cmake)
useOpenGL()

set(PROJECT_DEPS ${PROJECT_DEPS} sandbox)
set(DEP_LIBS ${DEP_LIBS} sandbox)

# --------------------- Executable -----------------------

addStandardExecutable()

