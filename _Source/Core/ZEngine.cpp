/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZEngine.cpp

    Created by Adrian Sanchez on 1/25/19.
    Copyright © 2019 Pervasive Sense. All rights reserved.

  This file is part of Zenith.

  Zenith is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Zenith is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Zenith.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "ZEngine.hpp"
#include "ZGame.hpp"
#include "ZGraphics.hpp"
#include "ZDomain.hpp"
#include "ZGLInput.hpp"
#include "ZUI.hpp"
#include "ZPhysics.hpp"
#include "ZIDSequence.hpp"
#include "ZOFTree.hpp"
#include "ZOFParser.hpp"
#include "ZProcessRunner.hpp"
#include "ZResourceCache.hpp"
#include "ZEventAgent.hpp"
#include "ZGOFactory.hpp"
#include "ZGraphicsFactory.hpp"
#include "ZUIFactory.hpp"
#include "ZPhysicsFactory.hpp"
#include "ZZipFile.hpp"

#include "ZScriptExports.hpp"
#include "ZLuaScriptManager.hpp"
#include "ZScriptResourceLoader.hpp"
#include "ZScriptableProcess.hpp"

const float ZEngine::DEFAULT_X_RESOLUTION = 2560.f;
const float ZEngine::DEFAULT_Y_RESOLUTION = 1600.f;
const float ZEngine::UPDATE_STEP_SIZE = 0.017f;
const int ZEngine::MAX_FIXED_UPDATE_ITERATIONS = 50;
const unsigned int ZEngine::SHADOW_MAP_SIZE = 1024;
const unsigned int ZEngine::CUBE_MAP_SIZE = 2048;
const unsigned int ZEngine::IRRADIANCE_MAP_SIZE = 32;
const unsigned int ZEngine::PREFILTER_MAP_SIZE = 128;
const unsigned int ZEngine::LUT_SIZE = 512;
const glm::vec4 ZEngine::WORLD_UP(0.0f, 1.0f, 0.0f, 0.0f);
const std::vector<std::string> ZEngine::DEFAULT_SKYBOX_CUBEMAP{
  "Assets/Skyboxes/Default/right.png",
  "Assets/Skyboxes/Default/left.png",
  "Assets/Skyboxes/Default/up.png",
  "Assets/Skyboxes/Default/down.png",
  "Assets/Skyboxes/Default/front.png",
  "Assets/Skyboxes/Default/back.png",
};
const std::string ZEngine::DEFAULT_HDR_CUBEMAP = "Assets/Skyboxes/DefaultHDR/sky.hdr";

std::shared_ptr<ZGame> ZEngine::currentGame_ = nullptr;
std::shared_ptr<ZDomain> ZEngine::domain_ = nullptr;
std::shared_ptr<ZGraphics> ZEngine::graphics_ = nullptr;
std::shared_ptr<ZInput> ZEngine::input_ = nullptr;
std::shared_ptr<ZUI> ZEngine::ui_ = nullptr;
std::shared_ptr<ZPhysics> ZEngine::physics_ = nullptr;
std::unique_ptr<ZProcessRunner> ZEngine::processRunner_ = nullptr;
std::shared_ptr<ZEventAgent> ZEngine::eventAgent_ = nullptr;
std::unique_ptr<ZResourceCache> ZEngine::resourceCache_ = nullptr;
std::unique_ptr<ZLuaScriptManager> ZEngine::scriptManager_ = nullptr;
std::unique_ptr<ZGOFactory> ZEngine::gameObjectFactory_ = nullptr;
std::unique_ptr<ZGraphicsFactory> ZEngine::graphicsFactory_ = nullptr;
std::unique_ptr<ZUIFactory> ZEngine::uiFactory_ = nullptr;
std::unique_ptr<ZPhysicsFactory> ZEngine::physicsFactory_ = nullptr;
float ZEngine::deltaTime_ = 0.0f;
float ZEngine::lastDeltaTime_ = 0.0f;
std::unique_ptr<ZIDSequence> ZEngine::idGenerator_(new ZIDSequence);

// TODO: Useful to have a config file to parse for more global state info such as window dimensions
// and maximum resource cache size
void ZEngine::Initialize(std::shared_ptr<ZGame> game, int windowWidth, int windowHeight) {
  currentGame_ = game;

  /* ========= Process System ============ */
  processRunner_.reset(new ZProcessRunner);
  /* ===================================== */

  /* ========= Resource Cache System ============ */
  resourceCache_.reset(new ZResourceCache(100));
	// TODO: Switch to development resource file based on compile flag
  resourceCache_->RegisterResourceFile(std::shared_ptr<ZZipFile>(new ZZipFile("Assets.zip")));  
  resourceCache_->Initialize();
  resourceCache_->RegisterLoader(std::shared_ptr<ZScriptResourceLoader>(new ZScriptResourceLoader));
  /* ============================================ */

  /* ========= Event System ============ */
  eventAgent_ = std::make_shared<ZEventAgent>();
  eventAgent_->Initialize();
  processRunner_->AttachProcess(eventAgent_);
  /* =================================== */

  /* ========= Scripting System ============ */
  scriptManager_.reset(new ZLuaScriptManager);
  scriptManager_->Initialize();
  ZScriptExports::Register();
  ZScriptableProcess::RegisterScriptClass();
  // We don't need to do anything with this resource. The resource loader
  // will load and execute the script for us.
  ZResource luaSetupScript("Assets/Scripts/init.lua");
  resourceCache_->GetHandle(&luaSetupScript);
  /* ======================================= */

  /* ========= Windowing System ============ */
  domain_ = std::make_shared<ZDomain>(windowWidth, windowHeight);
  domain_->Initialize();
  /* ======================================= */

  /* ========= Graphics System ============ */
  graphics_ = std::make_shared<ZGraphics>();
  graphics_->Initialize();
  /* ====================================== */

  /* ========= Input System ============ */
  input_ = std::make_shared<ZGLInput>();
  input_->Initialize();
  processRunner_->AttachProcess(input_);
  /* =================================== */

  /* ========= UI System ============ */
  ui_ = std::make_shared<ZUI>();
  ui_->Initialize();
  /* ================================ */

  /* ========= Physics System ============ */
  physics_ = std::make_shared<ZPhysics>();
  physics_->Initialize();
  processRunner_->AttachProcess(physics_);
  /* ===================================== */

  /* ========= Object Factories ============ */
  gameObjectFactory_.reset(new ZGOFactory);
  graphicsFactory_.reset(new ZGraphicsFactory);
  uiFactory_.reset(new ZUIFactory);
  physicsFactory_.reset(new ZPhysicsFactory);
  /* ======================================= */
}

std::shared_ptr<ZGame> ZEngine::Game() {
  return currentGame_;
}

ZDomain* ZEngine::Domain() {
  return domain_.get();
}

ZGraphics* ZEngine::Graphics() {
  return graphics_.get();
}

ZInput* ZEngine::Input() {
  return input_.get();
}

ZUI* ZEngine::UI() {
  return ui_.get();
}

ZPhysics* ZEngine::Physics() {
  return physics_.get();
}

ZProcessRunner* ZEngine::ProcessRunner() {
  return processRunner_.get();
}

ZResourceCache* ZEngine::ResourceCache() {
  return resourceCache_.get();
}

ZEventAgent* ZEngine::EventAgent() {
  return eventAgent_.get();
}

ZLuaScriptManager* ZEngine::ScriptManager() {
  return scriptManager_.get();
}

ZGOFactory* ZEngine::GameObjectFactory() {
  return gameObjectFactory_.get();
}

ZGraphicsFactory* ZEngine::GraphicsFactory() {
  return graphicsFactory_.get();
}

ZUIFactory* ZEngine::UIFactory() {
  return uiFactory_.get();
}

ZPhysicsFactory* ZEngine::PhysicsFactory() {
  return physicsFactory_.get();
}

ZIDSequence* ZEngine::IDSequence() {
  return idGenerator_.get();
}

float ZEngine::LastDeltaTime() {
  return lastDeltaTime_;
}

float ZEngine::DeltaTime() {
  return deltaTime_;
}

float ZEngine::SecondsTime() {
  using namespace std::chrono;
  return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() / 1000.0f;
}

void ZEngine::Provide(std::shared_ptr<ZDomain> domain) {
  if (domain_ != nullptr) {
    domain_ = nullptr;
  }
  domain_ = std::move(domain);
  domain_->Initialize();
}

void ZEngine::Provide(std::shared_ptr<ZGraphics> graphics) {
  if (graphics_ != nullptr) {
    graphics_ = nullptr;
  }
  graphics_ = std::move(graphics);
  graphics_->Initialize();
}

void ZEngine::Provide(std::shared_ptr<ZInput> input) {
  if (input_ != nullptr) {
    input_ = nullptr;
  }
  input_ = std::move(input);
}

void ZEngine::Provide(std::shared_ptr<ZUI> ui) {
  if (ui_ != nullptr) {
    ui_ = nullptr;
  }
  ui_ = std::move(ui);
  ui_->Initialize();
}

void ZEngine::Provide(std::shared_ptr<ZPhysics> physics) {
  if (physics_ != nullptr) {
    physics_ = nullptr;
  }
  physics_ = std::move(physics);
  physics_->Initialize();
}

void ZEngine::SetDeltaTime(float deltaTime) {
  lastDeltaTime_ = deltaTime_;
  deltaTime_ = deltaTime;
}

ZOFLoadResult ZEngine::LoadZOF(std::string zofPath) {
  ZOFLoadResult results;
  // TODO: ZOFTree should have append functionality so that different
  // ZOFTrees can be combined (by combining children into a single tree)
  ZOFParser parser;
  std::shared_ptr<ZOFTree> objectTree = parser.Parse(zofPath);

  // TODO: The more systems are populated this way, the more of a hamper we place on
  // load times. Refactor this so the object tree is only traversed once.
  if (scriptManager_ != nullptr) scriptManager_->Load(objectTree);
  if (graphics_ != nullptr) graphics_->Load(objectTree);

  results.gameObjects = gameObjectFactory_->Load(objectTree);
  results.uiElements = uiFactory_->Load(objectTree);

  return results;
}

void ZEngine::CleanUp() {
  processRunner_->AbortAllProcesses(true);

	physics_->CleanUp(); physics_.reset();
	ui_->CleanUp(); ui_.reset();
	input_.reset();
	graphics_->CleanUp(); graphics_.reset();
	domain_->CleanUp(); domain_.reset();
	resourceCache_.reset();
	eventAgent_->CleanUp();
	scriptManager_.reset();
}
