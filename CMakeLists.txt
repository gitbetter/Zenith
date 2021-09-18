cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 17)

set(ZENITH_LIB Zenith)
set(ENGINE_DIRECTORY ${CMAKE_SOURCE_DIR}/Engine)

project(${ZENITH_LIB})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ENGINE_DIRECTORY}/_Bin/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ENGINE_DIRECTORY}/_Bin/build)

option(DEVELOPMENT "Generate a development build" OFF)
set(USER_PROJECT_NAME "" CACHE STRING "Name of user project to generate")

add_definitions(-DENGINE_ROOT="${ENGINE_DIRECTORY}")
if (MSVC)
  add_definitions(/MP)
endif()

if(DEVELOPMENT)
  add_compile_definitions(DEV_BUILD)
endif(DEVELOPMENT)

include(${ENGINE_DIRECTORY}/CMakeLists.txt)
set(SOURCES ${ENGINE_SOURCES})
set(INCLUDES ${ENGINE_INCLUDES})

if (NOT HEADLESS)
  include(${ENGINE_DIRECTORY}/_Editor/CMakeLists.txt)
  list(APPEND SOURCES ${EDITOR_SOURCES})
  list(APPEND INCLUDES ${EDITOR_INCLUDES})
endif()

if (USER_PROJECT_NAME)
  add_library(${ZENITH_LIB} ${SOURCES})
else()
  add_executable(${ZENITH_LIB} ${SOURCES})
endif()

target_include_directories(${ZENITH_LIB} PUBLIC ${INCLUDES})

if (NOT DEVELOPMENT)
  file(GLOB RESOURCE_FILES
  "*.zip"
  )

  foreach(FILE ${RESOURCE_FILES})
    get_filename_component(FILENAME ${FILE} NAME)
    add_custom_command(TARGET ${ZENITH_LIB} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy ${FILE} ${ENGINE_DIRECTORY}/_Bin/${FILENAME}
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
  find_library(BULLETINVERSEDYNAMICS_BULLET_LIB NAMES BulletInverseDynamics HINTS "${DARWIN_LIB_DIR}")
  find_library(BULLETSOFTBODY_BULLET_LIB NAMES BulletSoftBody HINTS "${DARWIN_LIB_DIR}")
  find_library(FREETYPE_LIB NAMES freetyped HINTS "${DARWIN_LIB_DIR}")
  find_library(ZIP_LIB NAMES zip HINTS "${DARWIN_LIB_DIR}")
  find_library(LUA_LIB NAMES lua HINTS "${DARWIN_LIB_DIR}")
  find_library(CPPFS_LIB NAMES cppfsd HINTS "${DARWIN_LIB_DIR}")
  find_library(OGG_LIB NAMES ogg HINTS "${DARWIN_LIB_DIR}")
  find_library(VORBIS_LIB NAMES vorbis HINTS "${DARWIN_LIB_DIR}")
  find_library(VORBISFILE_LIB NAMES vorbisfile HINTS "${DARWIN_LIB_DIR}")
  find_library(OPENAL_LIB NAMES openal HINTS "${DARWIN_LIB_DIR}")
  find_library(RTTR_LIB NAMES librttr_core_d HINTS "${DARWIN_LIB_DIR}")

  set(LINKED_LIBS
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
  ${BULLETINVERSEDYNAMICS_BULLET_LIB}
  ${BULLETSOFTBODY_BULLET_LIB}
  ${OGG_LIB}
  ${VORBIS_LIB}
  ${VORBISFILE_LIB}
  ${OPENAL_LIB}
  ${RTTR_LIB})

  target_link_libraries(${ZENITH_LIB} ${LINKED_LIBS})

  add_custom_command(
  	TARGET ${ZENITH_LIB} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Engine"
    COMMENT "create directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Engine"
  )

  set (source "${ENGINE_DIRECTORY}/_Assets")
  set (destination "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Engine/_Assets")
      add_custom_command(
      TARGET ${ZENITH_LIB} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E create_symlink ${source} ${destination} 
      DEPENDS ${destination}
      COMMENT "symbolically link engine assets folder from ${source} => ${destination}"
  )

  add_custom_command(TARGET ${ZENITH_LIB} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${DARWIN_LIB_DIR}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>"
  )

  if (NOT HEADLESS)
    add_custom_command(
      TARGET ${ZENITH_LIB} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Engine/_Editor"
      COMMENT "create directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Engine/_Editor"
    )

    SET(source "${ENGINE_DIRECTORY}/_Editor/_Assets")
    SET(destination "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Engine/_Editor/_Assets")
    add_custom_command(
        TARGET ${ZENITH_LIB} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink ${source} ${destination} 
        DEPENDS ${destination}
        COMMENT "symbolically link editor assets folder from ${source} => ${destination}"
    )
  endif()

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
  find_library(BULLETINVERSEDYNAMICS_BULLET_LIB NAMES BulletInverseDynamics HINTS "${WINDOWS_LIB_DIR}")
  find_library(BULLETSOFTBODY_BULLET_LIB NAMES BulletSoftBody HINTS "${WINDOWS_LIB_DIR}")
  find_library(FREETYPE_LIB NAMES freetyped HINTS "${WINDOWS_LIB_DIR}")
  find_library(ZIP_LIB NAMES zip HINTS "${WINDOWS_LIB_DIR}")
  find_library(LUA_LIB NAMES lua54 HINTS "${WINDOWS_LIB_DIR}")
  find_library(CPPFS_LIB NAMES cppfsd HINTS "${WINDOWS_LIB_DIR}")
  find_library(OGG_LIB NAMES ogg_static libogg_static HINTS "${WINDOWS_LIB_DIR}")
  find_library(VORBIS_LIB NAMES vorbis_static libvorbis_static HINTS "${WINDOWS_LIB_DIR}")
  find_library(VORBISFILE_LIB NAMES vorbisfile_static libvorbisfile_static HINTS "${WINDOWS_LIB_DIR}")
  find_library(OPENAL_LIB NAMES OpenAL32 HINTS "${WINDOWS_LIB_DIR}")
  find_library(RTTR_LIB NAMES librttr_core_d HINTS "${WINDOWS_LIB_DIR}")

  set(LINKED_LIBS
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
  ${BULLETINVERSEDYNAMICS_BULLET_LIB}
  ${BULLETSOFTBODY_BULLET_LIB}
  ${OGG_LIB}
  ${VORBIS_LIB}
  ${VORBISFILE_LIB}
  ${OPENAL_LIB}
  ${RTTR_LIB})

  target_link_libraries(${ZENITH_LIB} ${LINKED_LIBS})

  add_custom_command(TARGET ${ZENITH_LIB} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${WINDOWS_LIB_DIR}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>"
  )

endif (APPLE)

foreach(FILE ${SOURCES}) 
	get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)
	string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")
	string(REPLACE "/" "\\" GROUP "${GROUP}")
	source_group("${GROUP}" FILES "${FILE}")
endforeach()

if (USER_PROJECT_NAME AND (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${USER_PROJECT_NAME}"))
  # Generate project files and directories
  file(MAKE_DIRECTORY ${USER_PROJECT_NAME})
  file(MAKE_DIRECTORY ${USER_PROJECT_NAME}/Headers)
  file(MAKE_DIRECTORY ${USER_PROJECT_NAME}/Source)
  file(MAKE_DIRECTORY ${USER_PROJECT_NAME}/Assets)
  file(TOUCH ${USER_PROJECT_NAME}/Assets/scene.zof)
  file(TOUCH ${USER_PROJECT_NAME}/Headers/Game.hpp)
  file(WRITE ${USER_PROJECT_NAME}/Source/Game.cpp 
"#include \"ZServices.hpp\"
#include \"ZGame.hpp\"
#include \"ZScene.hpp\"
#include \"ZSceneReadyEvent.hpp\"
#include \"ZInput.hpp\"

void onSceneLoad(std::shared_ptr<ZSceneReadyEvent> event);

int main(int argc, const char* argv[]) {
    // Create a new game instance
    std::shared_ptr<ZGame> game = std::make_shared<ZGame>();

    // Initialize the engine before anything else
    ZGameOptions gameOptions;
    gameOptions.domain.width = 800;
    gameOptions.domain.height = 600;
    gameOptions.domain.maximized = false;
    game->Initialize(gameOptions);

    // Ater initializing the engine, we can access the underlying UI subsystem to register fonts
    ZServices::FontManager()->CreateAsync(\"/Fonts/earth_orbiter/earthorbiter.ttf\");

    // Load a default scene from the engine
    ZScene::LoadIn({ \"/demo_scene.zof\" });

    // Register delegate methods for specific UI events
    ZServices::EventAgent()->Subscribe(&onSceneLoad);

    // Free up window cursor. It is captured by default.
    ZServices::Input()->ReleaseCursor();

    // Create the game and start the main game loop. Nothing beyond this point will execute
    // for the duration of the game.
    game->Loop();

    // Make sure to clean up all resources after we're done
    game->CleanUp();

    return 0;
}

void onSceneLoad(std::shared_ptr<ZSceneReadyEvent> event)
{
    LOG(\"Scene '\" + event->Scene()->Name() + \"' loaded\", ZSeverity::Info);
    event->Scene()->Play();
}")
  # Generate user CMakeLists.txt
  file(STRINGS ${CMAKE_SOURCE_DIR}/CMakeLists.user.txt USER_CMAKELISTS)
  string(REPLACE ";" "\r\n" USER_CMAKELISTS "${USER_CMAKELISTS}")
  file(CONFIGURE OUTPUT ${CMAKE_SOURCE_DIR}/${USER_PROJECT_NAME}/CMakeLists.txt
  CONTENT ${USER_CMAKELISTS} @ONLY
  NEWLINE_STYLE CRLF)
endif()
