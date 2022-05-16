# add_subdirectory(bezier_curve)
# add_subdirectory(opengl-bezier-surface)


cmake_minimum_required(VERSION 2.6)
add_compile_options(-std=c++14)
project(bezier)
set (CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address  -lasan -lstdc++ ")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address  -lasan -lstdc++ ")
if (POLICY CMP0072)
  set(OpenGL_GL_PREFERENCE LEGACY)
endif()
find_package(OpenGL REQUIRED)

find_package(PCL  REQUIRED)
include_directories(${PCL_INCLUDE_DIRS} ${Boost_INCLUDE_DIR})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

include_directories(${OpenGL_INCLUDE_DIR})
# find_package(FREEGLUT REQUIRED)
# include_directories(${FREEGLUT_INCLUDE_DIR})
## OpenGL
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
find_package(OpenCV REQUIRED)
include_directories("/usr/include/eigen3/" ${OpenCV_INCLUDE_DIRS} ..)


find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
find_package(glfw3 REQUIRED)
# include_directories(${GLFW3_INCLUDE_DIRS})
# message(${GLFW_INCLUDE_DIRS})
FIND_PACKAGE(GLUT REQUIRED)
INCLUDE_DIRECTORIES(${GLUT_INCLUDE_DIR}/src/
/home/u20/下载/NURBS曲面/src/

)
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIR})
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
## OpenGL
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
 

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
find_package(glfw3 REQUIRED)
# include_directories(${GLFW3_INCLUDE_DIRS})
# message(${GLFW_INCLUDE_DIRS})
FIND_PACKAGE(GLUT REQUIRED)
INCLUDE_DIRECTORIES(${GLUT_INCLUDE_DIR}/src/
${PROJECT_SOURCE_DIR}/src
/usr/include/eigen3
)

file(GLOB SRC  ./src/*.cpp)
# message(  )
add_executable(kriging kriging.cc ${SRC})
target_link_libraries(kriging  
${GLUT_LIBRARY} 
${OPENGL_LIBRARY}  
glfw
${OpenCV_LIBS}
${GLEW_LIBRARIES}
${PCL_LIBRARIES} ${Boost_LIBRARIES}
)

add_executable(tree tree/reatree.cc )
add_executable(4 tree/4.cc )