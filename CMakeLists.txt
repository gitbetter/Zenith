cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 14)

project(Zenith VERSION 1.0.0 LANGUAGES CXX)

option(DEVELOPMENT "Generate a development build" OFF)

set(PROJECT_NAME Zenith)

include(${CMAKE_SOURCE_DIR}/Engine/CMakeLists.txt)
include(${CMAKE_SOURCE_DIR}/Editor/CMakeLists.txt)

set(SOURCES ${ENGINE_SOURCES} ${EDITOR_SOURCES})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/_Bin/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/_Bin/build)

if(DEVELOPMENT)
	add_compile_definitions(DEV_BUILD)
endif(DEVELOPMENT)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_INCLUDES} ${EDITOR_INCLUDES})

if (NOT DEVELOPMENT)
  file(GLOB RESOURCE_FILES
  "*.zip"
  )

  foreach(FILE ${RESOURCE_FILES})
    get_filename_component(FILENAME ${FILE} NAME)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy ${FILE} ${CMAKE_SOURCE_DIR}/_Bin/${FILENAME}
                      )
  endforeach()
endif()

if (APPLE) 
  find_library(GLEW_LIB NAMES GLEW GLEWd HINTS "${DARWIN_LIB_DIR}")
  find_library(GLFW_LIB NAMES glfw HINTS "${DARWIN_LIB_DIR}")
  find_library(BZ2_LIB NAMES bz2 HINTS "${DARWIN_LIB_DIR}")
  find_library(ZLIB_LIB NAMES z HINTS "${DARWIN_LIB_DIR}")
  find_library(ASSIMP_LIB NAMES assimp HINTS "${DARWIN_LIB_DIR}")
  find_library(LINEARMATH_BULLET_LIB NAMES LinearMath HINTS "${DARWIN_LIB_DIR}")
  find_library(BULLETCOLLISION_BULLET_LIB NAMES BulletCollision HINTS "${DARWIN_LIB_DIR}")
  find_library(BULLETDYNAMICS_BULLET_LIB NAMES BulletDynamics HINTS "${DARWIN_LIB_DIR}")
  find_library(BULLETDYNAMICS_BULLET_LIB NAMES BulletDynamics HINTS "${DARWIN_LIB_DIR}")
  find_library(FREETYPE_LIB NAMES freetyped HINTS "${DARWIN_LIB_DIR}")
  find_library(ZIP_LIB NAMES zip HINTS "${DARWIN_LIB_DIR}")
  find_library(LUA_LIB NAMES lua HINTS "${DARWIN_LIB_DIR}")
  find_library(CPPFS_LIB NAMES cppfsd HINTS "${DARWIN_LIB_DIR}")
  find_library(OGG_LIB NAMES ogg HINTS "${DARWIN_LIB_DIR}")
  find_library(VORBIS_LIB NAMES vorbis HINTS "${DARWIN_LIB_DIR}")
  find_library(VORBISFILE_LIB NAMES vorbisfile HINTS "${DARWIN_LIB_DIR}")
  find_library(OPENAL_LIB NAMES openal HINTS "${DARWIN_LIB_DIR}")

  target_link_libraries(${PROJECT_NAME} 
  "-framework Cocoa" 
  "-framework OpenGL" 
  "-framework IOKit" 
  "-framework CoreVideo" 
  "-framework CoreFoundation"
  ${GLEW_LIB}
  ${GLFW_LIB}
  ${CPPFS_LIB}
  ${BZ2_LIB}
  ${ZLIB_LIB}
  ${FREETYPE_LIB} 
  ${ZIP_LIB} 
  ${LUA_LIB}
  ${ASSIMP_LIB} 
  ${LINEARMATH_BULLET_LIB} 
  ${BULLETCOLLISION_BULLET_LIB} 
  ${BULLETDYNAMICS_BULLET_LIB}
  ${OGG_LIB}
  ${VORBIS_LIB}
  ${VORBISFILE_LIB}
  ${OPENAL_LIB})

  add_custom_command(
  	TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Editor"
    COMMENT "create directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Editor"
  )

  SET(source "${CMAKE_SOURCE_DIR}/Editor/_Assets")
  SET(destination "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Editor/_Assets")
  add_custom_command(
      TARGET ${PROJECT_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E create_symlink ${source} ${destination} 
      DEPENDS ${destination}
      COMMENT "symbolically link editor assets folder from ${source} => ${destination}"
  )

  add_custom_command(
  	TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Engine"
    COMMENT "create directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Engine"
  )

  set (source "${CMAKE_SOURCE_DIR}/Engine/_Assets")
  set (destination "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Engine/_Assets")
      add_custom_command(
      TARGET ${PROJECT_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E create_symlink ${source} ${destination} 
      DEPENDS ${destination}
      COMMENT "symbolically link engine assets folder from ${source} => ${destination}"
  )

  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${DARWIN_LIB_DIR}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>"
  )

elseif (WIN32)
  find_package(OpenGL REQUIRED)
  find_library(GLEW_LIB NAMES glew32 HINTS "${WINDOWS_LIB_DIR}")
  find_library(GLFW_LIB NAMES glfw3 HINTS "${WINDOWS_LIB_DIR}")
  find_library(BZ2_LIB NAMES bzip2 HINTS "${WINDOWS_LIB_DIR}")
  find_library(ZLIB_LIB NAMES zlibstaticd HINTS "${WINDOWS_LIB_DIR}")
  find_library(ASSIMP_LIB NAMES assimp HINTS "${WINDOWS_LIB_DIR}")
  find_library(LINEARMATH_BULLET_LIB NAMES LinearMath HINTS "${WINDOWS_LIB_DIR}")
  find_library(BULLETCOLLISION_BULLET_LIB NAMES BulletCollision HINTS "${WINDOWS_LIB_DIR}")
  find_library(BULLETDYNAMICS_BULLET_LIB NAMES BulletDynamics HINTS "${WINDOWS_LIB_DIR}")
  find_library(BULLETDYNAMICS_BULLET_LIB NAMES BulletDynamics HINTS "${WINDOWS_LIB_DIR}")
  find_library(FREETYPE_LIB NAMES freetyped HINTS "${WINDOWS_LIB_DIR}")
  find_library(ZIP_LIB NAMES zip HINTS "${WINDOWS_LIB_DIR}")
  find_library(LUA_LIB NAMES lua53 HINTS "${WINDOWS_LIB_DIR}")
  find_library(CPPFS_LIB NAMES cppfsd HINTS "${WINDOWS_LIB_DIR}")
  find_library(OGG_LIB NAMES ogg_static libogg_static HINTS "${WINDOWS_LIB_DIR}")
  find_library(VORBIS_LIB NAMES vorbis_static libvorbis_static HINTS "${WINDOWS_LIB_DIR}")
  find_library(VORBISFILE_LIB NAMES vorbisfile_static libvorbisfile_static HINTS "${WINDOWS_LIB_DIR}")
  find_library(OPENAL_LIB NAMES OpenAL32 HINTS "${WINDOWS_LIB_DIR}")

  target_link_libraries(${PROJECT_NAME} 
  ${OPENGL_LIBRARY} 
  gdi32 
  ${GLEW_LIB}
  ${GLFW_LIB}
  ${CPPFS_LIB}
  ${BZ2_LIB}
  ${ZLIB_LIB}
  ${FREETYPE_LIB} 
  ${ZIP_LIB} 
  ${LUA_LIB}
  ${ASSIMP_LIB} 
  ${LINEARMATH_BULLET_LIB} 
  ${BULLETCOLLISION_BULLET_LIB} 
  ${BULLETDYNAMICS_BULLET_LIB}
  ${OGG_LIB}
  ${VORBIS_LIB}
  ${VORBISFILE_LIB}
  ${OPENAL_LIB})

  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${WINDOWS_LIB_DIR}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>"
  )

endif (APPLE)

foreach(FILE ${SOURCES}) 
	get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)
	string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")
	string(REPLACE "/" "\\" GROUP "${GROUP}")
	source_group("${GROUP}" FILES "${FILE}")
endforeach()
