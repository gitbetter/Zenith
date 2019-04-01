cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 14)

project(Zenith VERSION 1.0.0.0 LANGUAGES CXX)

include_directories(${CMAKE_SOURCE_DIR}/_Headers)
link_directories(${CMAKE_SOURCE_DIR}/_Drivers/lib)

file(GLOB_RECURSE Zenith_SRCS "*.cpp")

add_executable(zenith ${Zenith_SRCS})

target_link_libraries(zenith glew glfw bz2 z)

if (APPLE)
  target_link_libraries(zenith "-framework Cocoa" 
                               "-framework OpenGL" 
                               "-framework IOKit" 
                               "-framework CoreVideo" 
                               "-framework CoreFoundation")
endif (APPLE)

if (WIN32)
  target_link_libraries(zenith assimp LinearMath BulletCollision BulletDynamics)
elseif (CYGWIN)
  target_link_libraries(zenith GL gdi32 freetyped_cyg zip_cyg lua53)
elseif (UNIX)
  target_link_libraries(zenith freetyped zip lua)
endif (WIN32)

target_link_libraries(zenith assimp LinearMath BulletCollision BulletDynamics)