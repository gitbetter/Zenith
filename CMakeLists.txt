cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 14)

project(Zenith VERSION 1.0.0.0 LANGUAGES CXX)

set(DARWIN_LIB_DIR ${CMAKE_SOURCE_DIR}/_Drivers/darwin)
set(WINDOWS_LIB_DIR ${CMAKE_SOURCE_DIR}/_Drivers/win64)
set(HEADERS_DIR ${CMAKE_SOURCE_DIR}/_Headers)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/_Source)

file(GLOB RESOURCE_FILES
  "*.zip" "*.zof"
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/_Bin/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/_Bin/build)

add_executable(zenith 
  ${HEADERS_DIR}/_ThirdParty/glm/detail/glm.cpp
  ${SOURCE_DIR}/UI/ZUIFactory.cpp
  ${SOURCE_DIR}/UI/ZUIElement.cpp
  ${SOURCE_DIR}/UI/ZMeshUI.cpp
  ${SOURCE_DIR}/UI/Elements/ZUICursor.cpp
  ${SOURCE_DIR}/UI/Elements/ZUICheckBox.cpp
  ${SOURCE_DIR}/UI/Elements/ZUIText.cpp
  ${SOURCE_DIR}/UI/Elements/ZUIListPanel.cpp
  ${SOURCE_DIR}/UI/Elements/ZUIPanel.cpp
  ${SOURCE_DIR}/UI/Elements/ZUIImage.cpp
  ${SOURCE_DIR}/UI/Elements/ZUIButton.cpp
  ${SOURCE_DIR}/UI/ZUI.cpp
  ${SOURCE_DIR}/Scripting/ZScriptManager.cpp
  ${SOURCE_DIR}/Scripting/ZScriptExports.cpp
  ${SOURCE_DIR}/Scripting/ZLuaScriptManager.cpp
  ${SOURCE_DIR}/Core/ZGame.cpp
  ${SOURCE_DIR}/Core/ZEngine.cpp
  ${SOURCE_DIR}/Input/ZGLInput.cpp
  ${SOURCE_DIR}/Input/ZInput.cpp
  ${SOURCE_DIR}/ResourceCache/ResourceLoaders/ZScriptResourceLoader.cpp
  ${SOURCE_DIR}/ResourceCache/ResourceLoaders/ZDefaultResourceLoader.cpp
  ${SOURCE_DIR}/ResourceCache/ZResourceHandle.cpp
  ${SOURCE_DIR}/ResourceCache/ZResourceCache.cpp
  ${SOURCE_DIR}/ResourceCache/ResourceFiles/ZZipFile.cpp
  ${SOURCE_DIR}/ResourceCache/ZResourceLoader.cpp
  ${SOURCE_DIR}/ResourceCache/ZResource.cpp
  ${SOURCE_DIR}/ResourceCache/ZResourceFile.cpp
  ${SOURCE_DIR}/Components/ZCameraComponent.cpp
  ${SOURCE_DIR}/Components/ZScriptComponent.cpp
  ${SOURCE_DIR}/Components/ZComponent.cpp
  ${SOURCE_DIR}/Components/ZGraphicsComponent.cpp
  ${SOURCE_DIR}/Components/ZPhysicsComponent.cpp
  ${SOURCE_DIR}/GameObjects/ZLight.cpp
  ${SOURCE_DIR}/GameObjects/ZGameObject.cpp
  ${SOURCE_DIR}/GameObjects/ZGOFactory.cpp
  ${SOURCE_DIR}/GameObjects/ZSkybox.cpp
  ${SOURCE_DIR}/GameObjects/ZParticleSystem.cpp
  ${SOURCE_DIR}/GameObjects/ZParticle.cpp
  ${SOURCE_DIR}/Windowing/ZGLDomainStrategy.cpp
  ${SOURCE_DIR}/Windowing/ZDomainStrategy.cpp
  ${SOURCE_DIR}/Windowing/ZDomain.cpp
  ${SOURCE_DIR}/Physics/Forces/ZAnchoredSpringForce.cpp
  ${SOURCE_DIR}/Physics/Forces/ZGravityForce.cpp
  ${SOURCE_DIR}/Physics/Forces/ZBungeeForce.cpp
  ${SOURCE_DIR}/Physics/Forces/ZBuoyancyForce.cpp
  ${SOURCE_DIR}/Physics/Forces/ZStiffSpringForce.cpp
  ${SOURCE_DIR}/Physics/Forces/ZSpringForce.cpp
  ${SOURCE_DIR}/Physics/Forces/ZDragForce.cpp
  ${SOURCE_DIR}/Physics/Forces/ZForceGenerator.cpp
  ${SOURCE_DIR}/Physics/Forces/ZObjectForceRegistry.cpp
  ${SOURCE_DIR}/Physics/ZPhysics.cpp
  ${SOURCE_DIR}/Graphics/ZMaterial.cpp
  ${SOURCE_DIR}/Graphics/ZMesh3D.cpp
  ${SOURCE_DIR}/Graphics/ZGraphics.cpp
  ${SOURCE_DIR}/Graphics/Strategies/ZGraphicsStrategy.cpp
  ${SOURCE_DIR}/Graphics/Strategies/ZGLGraphicsStrategy.cpp
  ${SOURCE_DIR}/Graphics/Strategies/ZGLTextStrategy.cpp
  ${SOURCE_DIR}/Graphics/Strategies/ZTextStrategy.cpp
  ${SOURCE_DIR}/Graphics/ZShader.cpp
  ${SOURCE_DIR}/Graphics/ZGraphicsFactory.cpp
  ${SOURCE_DIR}/Graphics/ZMesh.cpp
  ${SOURCE_DIR}/Graphics/ZPhysicsDebug.cpp
  ${SOURCE_DIR}/Graphics/ZModel.cpp
  ${SOURCE_DIR}/EventAgent/ZEventAgent.cpp
  ${SOURCE_DIR}/EventAgent/ZEvent.cpp
  ${SOURCE_DIR}/EventAgent/Events/ZObjectLookEvent.cpp
  ${SOURCE_DIR}/EventAgent/Events/ZObjectMoveEvent.cpp
  ${SOURCE_DIR}/EventAgent/Events/ZQuitEvent.cpp
  ${SOURCE_DIR}/EventAgent/Events/ZRaycastEvent.cpp
  ${SOURCE_DIR}/EventAgent/Events/ZFireEvent.cpp
  ${SOURCE_DIR}/EventAgent/Events/ZObjectSelectedEvent.cpp
  ${SOURCE_DIR}/EventAgent/Events/ZObjectDragEvent.cpp
  ${SOURCE_DIR}/EventAgent/ZScriptableEvent.cpp
  ${SOURCE_DIR}/Process/ZProcessRunner.cpp
  ${SOURCE_DIR}/Process/ZProcess.cpp
  ${SOURCE_DIR}/Process/ZScriptableProcess.cpp
  ${SOURCE_DIR}/Utility/ZObjectFormatTools/ZOFParser.cpp
  ${SOURCE_DIR}/Utility/ZGLModelImporter.cpp
  ${SOURCE_DIR}/Utility/ZIDSequence.cpp
  ${SOURCE_DIR}/Utility/stb_image.cpp
  ${SOURCE_DIR}/main.cpp
)

target_include_directories(zenith PUBLIC 
  ${HEADERS_DIR}
  ${HEADERS_DIR}/_ThirdParty
  ${HEADERS_DIR}/_ThirdParty/assimp
  ${HEADERS_DIR}/_ThirdParty/assimp/Compiler
  ${HEADERS_DIR}/_ThirdParty/bullet
  ${HEADERS_DIR}/_ThirdParty/bullet/BulletCollision
  ${HEADERS_DIR}/_ThirdParty/bullet/BulletDynamics
  ${HEADERS_DIR}/_ThirdParty/bullet/LinearMath
  ${HEADERS_DIR}/_ThirdParty/bzip2
  ${HEADERS_DIR}/_ThirdParty/fastdelegate
  ${HEADERS_DIR}/_ThirdParty/freetype
  ${HEADERS_DIR}/_ThirdParty/freetype/config
  ${HEADERS_DIR}/_ThirdParty/freetype/internal
  ${HEADERS_DIR}/_ThirdParty/GL
  ${HEADERS_DIR}/_ThirdParty/GLFW
  ${HEADERS_DIR}/_ThirdParty/glm
  ${HEADERS_DIR}/_ThirdParty/glm/detail
  ${HEADERS_DIR}/_ThirdParty/glm/ext
  ${HEADERS_DIR}/_ThirdParty/glm/gtc
  ${HEADERS_DIR}/_ThirdParty/glm/gtx
  ${HEADERS_DIR}/_ThirdParty/glm/simd
  ${HEADERS_DIR}/_ThirdParty/lua
  ${HEADERS_DIR}/_ThirdParty/zip
  ${HEADERS_DIR}/_ThirdParty/zlib
  ${HEADERS_DIR}/Components
  ${HEADERS_DIR}/Core
  ${HEADERS_DIR}/EventAgent
  ${HEADERS_DIR}/EventAgent/Events
  ${HEADERS_DIR}/GameObjects
  ${HEADERS_DIR}/Graphics
  ${HEADERS_DIR}/Graphics/Strategies
  ${HEADERS_DIR}/Input
  ${HEADERS_DIR}/Physics
  ${HEADERS_DIR}/Physics/Forces
  ${HEADERS_DIR}/Process
  ${HEADERS_DIR}/ResourceCache
  ${HEADERS_DIR}/ResourceCache/ResourceFiles
  ${HEADERS_DIR}/ResourceCache/ResourceLoaders
  ${HEADERS_DIR}/Scripting
  ${HEADERS_DIR}/UI
  ${HEADERS_DIR}/UI/Elements
  ${HEADERS_DIR}/Utility
  ${HEADERS_DIR}/Utility/ZObjectFormatTools
  ${HEADERS_DIR}/Windowing
)

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

  target_link_libraries(zenith "-framework Cocoa" 
                               "-framework OpenGL" 
                               "-framework IOKit" 
                               "-framework CoreVideo" 
                               "-framework CoreFoundation"
                               ${GLEW_LIB}
                               ${GLFW_LIB}
                               ${BZ2_LIB}
                               ${ZLIB_LIB}
                               ${FREETYPE_LIB} 
                               ${ZIP_LIB} 
                               ${LUA_LIB}
                               ${ASSIMP_LIB} 
                               ${LINEARMATH_BULLET_LIB} 
                               ${BULLETCOLLISION_BULLET_LIB} 
                               ${BULLETDYNAMICS_BULLET_LIB})

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

  target_link_libraries(zenith ${OPENGL_LIBRARY} 
                               gdi32 
                               ${GLEW_LIB}
                               ${GLFW_LIB}
                               ${BZ2_LIB}
                               ${ZLIB_LIB}
                               ${FREETYPE_LIB} 
                               ${ZIP_LIB} 
                               ${LUA_LIB}
                               ${ASSIMP_LIB} 
                               ${LINEARMATH_BULLET_LIB} 
                               ${BULLETCOLLISION_BULLET_LIB} 
                               ${BULLETDYNAMICS_BULLET_LIB})

  add_custom_command(TARGET zenith POST_BUILD
                     COMMAND "${CMAKE_COMMAND}" -E copy_directory "${WINDOWS_LIB_DIR}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>"
                    )

  foreach(FILE ${RESOURCE_FILES})
    get_filename_component(FILENAME ${FILE} NAME)
    add_custom_command(TARGET zenith POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy ${FILE} ${CMAKE_SOURCE_DIR}/_Bin/${FILENAME}
                      )
  endforeach()
endif (APPLE)