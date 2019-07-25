project (ExampleApp)

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
useNanoGUI()

include(${external_dir}/CPP11/CPP11.cmake)
useCPP11()

include(${external_dir}/OpenGL/OpenGL.cmake)
useOpenGL()

set(PROJECT_DEPS ${PROJECT_DEPS} sandbox)
set(DEP_LIBS ${DEP_LIBS} sandbox)

# --------------------- Executable -----------------------

addStandardExecutable()

