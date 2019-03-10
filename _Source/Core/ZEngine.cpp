//
//  ZEngine.cpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#include "ZEngine.hpp"
#include "ZGame.hpp"
#include "ZGraphics.hpp"
#include "ZDomain.hpp"
#include "ZNullInput.hpp"
#include "ZGLInput.hpp"
#include "ZUI.hpp"
#include "ZPhysics.hpp"
#include "ZIDSequence.hpp"
#include "ZOFTree.hpp"
#include "ZOFParser.hpp"
#include "ZGOFactory.hpp"
#include "ZGraphicsFactory.hpp"
#include "ZResourceCache.hpp"
#include "ZZipFile.hpp"
#include <chrono>

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
std::unique_ptr<ZDomain> ZEngine::domain_ = nullptr;
std::unique_ptr<ZGraphics> ZEngine::graphics_ = nullptr;
std::unique_ptr<ZInput> ZEngine::input_(new ZNullInput);
std::unique_ptr<ZUI> ZEngine::ui_ = nullptr;
std::unique_ptr<ZPhysics> ZEngine::physics_ = nullptr;
std::unique_ptr<ZResourceCache> ZEngine::resourceCache_ = nullptr;
std::unique_ptr<ZGOFactory> ZEngine::gameObjectFactory_ = nullptr;
std::unique_ptr<ZGraphicsFactory> ZEngine::graphicsFactory_ = nullptr;
float ZEngine::deltaTime_ = 0.0f;
std::unique_ptr<ZIDSequence> ZEngine::idGenerator_(new ZIDSequence);

// TODO: Useful to have a config file to parse for more global state info such as window dimensions
// and maximum resource cache size
void ZEngine::Initialize(std::shared_ptr<ZGame> game, int windowWidth, int windowHeight) {
  currentGame_ = game;

  resourceCache_.reset(new ZResourceCache(100));
  resourceCache_->RegisterResourceFile(std::shared_ptr<ZZipFile>(new ZZipFile("Assets.zip")));
  resourceCache_->Initialize();

  domain_.reset(new ZDomain(windowWidth, windowHeight));
  domain_->Initialize();

  graphics_.reset(new ZGraphics);
  graphics_->Initialize();

  input_.reset(new ZGLInput);
  input_->Register(currentGame_);

  ui_.reset(new ZUI);
  ui_->Initialize();

  physics_.reset(new ZPhysics);
  physics_->Initialize();

  gameObjectFactory_.reset(new ZGOFactory);
  graphicsFactory_.reset(new ZGraphicsFactory);
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

ZResourceCache* ZEngine::ResourceCache() {
  return resourceCache_.get();
}

ZGOFactory* ZEngine::GameObjectFactory() {
  return gameObjectFactory_.get();
}

ZGraphicsFactory* ZEngine::GraphicsFactory() {
  return graphicsFactory_.get();
}

ZIDSequence* ZEngine::IDSequence() {
  return idGenerator_.get();
}

float ZEngine::DeltaTime() {
  return deltaTime_;
}

float ZEngine::MilliSecondTime() {
  using namespace std::chrono;
  return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() / 1000.0f;
}

void ZEngine::Provide(std::unique_ptr<ZDomain> domain) {
  if (domain_ != nullptr) {
    domain_ = nullptr;
  }
  domain_ = std::move(domain);
  domain_->Initialize();
}

void ZEngine::Provide(std::unique_ptr<ZGraphics> graphics) {
  if (graphics_ != nullptr) {
    graphics_ = nullptr;
  }
  graphics_ = std::move(graphics);
  graphics_->Initialize();
}

void ZEngine::Provide(std::unique_ptr<ZInput> input) {
  // If the provided input object is not null and the existing engine input object
  // is null, delete the existing one
  if (!dynamic_cast<ZNullInput*>(input.get()) && dynamic_cast<ZNullInput*>(input_.get())) {
    input_ = nullptr;
  }
  input_ = std::move(input);
}

void ZEngine::Provide(std::unique_ptr<ZUI> ui) {
  if (ui_ != nullptr) {
    ui_ = nullptr;
  }
  ui_ = std::move(ui);
  ui_->Initialize();
}

void ZEngine::Provide(std::unique_ptr<ZPhysics> physics) {
  if (physics_ != nullptr) {
    physics_ = nullptr;
  }
  physics_ = std::move(physics);
  physics_->Initialize();
}

void ZEngine::SetDeltaTime(float deltaTime) {
  deltaTime_ = deltaTime;
}

ZGameObjectMap ZEngine::LoadZOF(std::string zofPath) {
  // TODO: ZOFTree should have append functionality so that different
  // ZOFTrees can be combined (by combining children into a single tree)
  ZOFParser parser;
  ZOFTree* objectTree = parser.Parse(zofPath);

  if (graphics_ != nullptr) graphics_->Load(objectTree);

  return gameObjectFactory_->Create(objectTree);
  // TODO: uiFactory_->Create(objectTree)
}
