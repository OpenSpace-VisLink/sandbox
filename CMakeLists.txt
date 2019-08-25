project(ExampleProject)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_FRAMEWORK_DIR ${CMAKE_SOURCE_DIR}/cmake/framework)

if(NOT EXISTS ${CMAKE_FRAMEWORK_DIR})
execute_process(
		#COMMAND git clone ${PROJECT_SOURCE_DIR}/.. ${CMAKE_FRAMEWORK_DIR}
		COMMAND git clone https://github.com/dtorban/cmake_fw.git ${CMAKE_FRAMEWORK_DIR}
)
endif()

include(${CMAKE_FRAMEWORK_DIR}/project/MainProject.cmake)

option(WITH_APPS "If enabled, includes apps in the build." ON)

add_subdirectory(src/sandbox)
if(${WITH_APPS})
	add_subdirectory(app)
endif()
#add_subdirectory(examples/ScatterPlot)
#add_subdirectory(examples/FunctionView)
add_subdirectory(examples/VulkanExample)
add_subdirectory(examples/VulkanSandbox)
