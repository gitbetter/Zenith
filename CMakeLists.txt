cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 14)

project(Zenith VERSION 1.0.0.0 LANGUAGES CXX)

SET(LIB_DIR ${CMAKE_SOURCE_DIR}/_Drivers/lib)
SET(HEADERS_DIR ${CMAKE_SOURCE_DIR}/_Headers)
SET(SOURCE_DIR ${CMAKE_SOURCE_DIR}/_Source)

include_directories(${HEADERS_DIR})
include_directories(${HEADERS_DIR}/_ThirdParty)
include_directories(${HEADERS_DIR}/_ThirdParty/assimp)
include_directories(${HEADERS_DIR}/_ThirdParty/assimp/Compiler)
include_directories(${HEADERS_DIR}/_ThirdParty/bullet)
include_directories(${HEADERS_DIR}/_ThirdParty/bullet/BulletCollision)
include_directories(${HEADERS_DIR}/_ThirdParty/bullet/BulletDynamics)
include_directories(${HEADERS_DIR}/_ThirdParty/bullet/LinearMath)
include_directories(${HEADERS_DIR}/_ThirdParty/bzip2)
include_directories(${HEADERS_DIR}/_ThirdParty/fastdelegate)
include_directories(${HEADERS_DIR}/_ThirdParty/freetype)
include_directories(${HEADERS_DIR}/_ThirdParty/freetype/config)
include_directories(${HEADERS_DIR}/_ThirdParty/freetype/internal)
include_directories(${HEADERS_DIR}/_ThirdParty/GL)
include_directories(${HEADERS_DIR}/_ThirdParty/GLFW)
include_directories(${HEADERS_DIR}/_ThirdParty/glm)
include_directories(${HEADERS_DIR}/_ThirdParty/glm/detail)
include_directories(${HEADERS_DIR}/_ThirdParty/glm/ext)
include_directories(${HEADERS_DIR}/_ThirdParty/glm/gtc)
include_directories(${HEADERS_DIR}/_ThirdParty/glm/gtx)
include_directories(${HEADERS_DIR}/_ThirdParty/glm/simd)
include_directories(${HEADERS_DIR}/_ThirdParty/lua)
include_directories(${HEADERS_DIR}/_ThirdParty/zip)
include_directories(${HEADERS_DIR}/_ThirdParty/zlib)
include_directories(${HEADERS_DIR}/Components)
include_directories(${HEADERS_DIR}/Core)
include_directories(${HEADERS_DIR}/EventAgent)
include_directories(${HEADERS_DIR}/EventAgent/Events)
include_directories(${HEADERS_DIR}/GameObjects)
include_directories(${HEADERS_DIR}/Graphics)
include_directories(${HEADERS_DIR}/Graphics/Strategies)
include_directories(${HEADERS_DIR}/Input)
include_directories(${HEADERS_DIR}/Physics)
include_directories(${HEADERS_DIR}/Physics/Forces)
include_directories(${HEADERS_DIR}/Process)
include_directories(${HEADERS_DIR}/ResourceCache)
include_directories(${HEADERS_DIR}/ResourceCache/ResourceFiles)
include_directories(${HEADERS_DIR}/ResourceCache/ResourceLoaders)
include_directories(${HEADERS_DIR}/Scripting)
include_directories(${HEADERS_DIR}/UI)
include_directories(${HEADERS_DIR}/UI/Elements)
include_directories(${HEADERS_DIR}/Utility)
include_directories(${HEADERS_DIR}/Utility/ZObjectFormatTools)
include_directories(${HEADERS_DIR}/Windowing)

# UNIX/APPLE Libraries
find_library(GLEW_LIB NAMES GLEW GLEWd HINTS "${LIB_DIR}")
find_library(GLFW_LIB NAMES glfw HINTS "${LIB_DIR}")
find_library(BZ2_LIB NAMES bz2 HINTS "${LIB_DIR}")
find_library(ZLIB_LIB NAMES z HINTS "${LIB_DIR}")
find_library(ASSIMP_LIB NAMES assimp HINTS "${LIB_DIR}")
find_library(LINEARMATH_BULLET_LIB NAMES LinearMath HINTS "${LIB_DIR}")
find_library(BULLETCOLLISION_BULLET_LIB NAMES BulletCollision HINTS "${LIB_DIR}")
find_library(BULLETDYNAMICS_BULLET_LIB NAMES BulletDynamics HINTS "${LIB_DIR}")
find_library(BULLETDYNAMICS_BULLET_LIB NAMES BulletDynamics HINTS "${LIB_DIR}")
find_library(FREETYPE_LIB NAMES freetyped HINTS "${LIB_DIR}")
find_library(ZIP_LIB NAMES zip HINTS "${LIB_DIR}")
find_library(LUA_LIB NAMES lua HINTS "${LIB_DIR}")

# CYGWIN Libraries
find_library(FREETYPECYG_LIB NAMES freetyped_cyg HINTS "${LIB_DIR}")
find_library(ZIPCYG_LIB NAMES zip_cyg HINTS "${LIB_DIR}")
find_library(LUACYG_LIB NAMES lua53 HINTS "${LIB_DIR}")

# Win32 Libraries


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
  ${SOURCE_DIR}/Utility/ZObjectFormatTools/ZOFTree.cpp
  ${SOURCE_DIR}/Utility/ZGLModelImporter.cpp
  ${SOURCE_DIR}/Utility/ZIDSequence.cpp
  ${SOURCE_DIR}/Utility/stb_image.cpp
  ${SOURCE_DIR}/main.cpp
)

target_link_libraries(zenith ${GLEW_LIB} 
                             ${GLFW_LIB} 
                             ${BZ2_LIB} 
                             ${ZLIB_LIB})

if (APPLE)
  target_link_libraries(zenith "-framework Cocoa" 
                               "-framework OpenGL" 
                               "-framework IOKit" 
                               "-framework CoreVideo" 
                               "-framework CoreFoundation")
endif (APPLE)

if (CYGWIN)
  target_link_libraries(zenith GL 
                               gdi32 
                               ${FREETYPECYG_LIB} 
                               ${ZIPCYG_LIB} 
                               ${LUACYG_LIB})
elseif (UNIX)
  target_link_libraries(zenith ${FREETYPE_LIB} 
                               ${ZIP_LIB} 
                               ${LUA_LIB})
endif (CYGWIN)

target_link_libraries(zenith ${ASSIMP_LIB} 
                             ${LINEARMATH_BULLET_LIB} 
                             ${BULLETCOLLISION_BULLET_LIB} 
                             ${BULLETDYNAMICS_BULLET_LIB})