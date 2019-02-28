//
//  ZEngine.cpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
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
#include <chrono>

const float ZEngine::UPDATE_STEP_SIZE = 0.017f;
const int ZEngine::MAX_FIXED_UPDATE_ITERATIONS = 50;
const unsigned int ZEngine::SHADOW_MAP_SIZE = 1024;
const glm::vec4 ZEngine::WORLD_UP(0.0f, 1.0f, 0.0f, 0.0f);
const std::vector<std::string> ZEngine::DEFAULT_SKYBOX_CUBEMAP{
  "Assets/Skyboxes/Default/right.png",
  "Assets/Skyboxes/Default/left.png",
  "Assets/Skyboxes/Default/up.png",
  "Assets/Skyboxes/Default/down.png",
  "Assets/Skyboxes/Default/front.png",
  "Assets/Skyboxes/Default/back.png",
};

ZGame* ZEngine::currentGame_ = nullptr;
ZDomain* ZEngine::domain_ = nullptr;
ZGraphics* ZEngine::graphics_ = nullptr;
ZInput* ZEngine::input_ = new ZNullInput;
ZUI* ZEngine::ui_ = nullptr;
ZPhysics* ZEngine::physics_ = nullptr;
ZGOFactory* ZEngine::gameObjectFactory_ = nullptr;
ZGraphicsFactory* ZEngine::graphicsFactory_ = nullptr;
float ZEngine::deltaTime_ = 0.0f;
ZIDSequence* ZEngine::idGenerator_ = new ZIDSequence;

void ZEngine::Initialize(ZGame* game, int windowWidth, int windowHeight) {
  currentGame_ = game;

  domain_ = new ZDomain(windowWidth, windowHeight);
  domain_->Initialize();

  graphics_ = new ZGraphics;
  graphics_->Initialize();

  input_ = new ZGLInput;
  input_->Register(currentGame_);

  ui_ = new ZUI;
  ui_->Initialize();

  physics_ = new ZPhysics;
  physics_->Initialize();
}

ZGame* ZEngine::Game() {
  return currentGame_;
}

ZDomain* ZEngine::Domain() {
  return domain_;
}

ZGraphics* ZEngine::Graphics() {
  return graphics_;
}

ZInput* ZEngine::Input() {
  return input_;
}

ZUI* ZEngine::UI() {
  return ui_;
}

ZPhysics* ZEngine::Physics() {
  return physics_;
}

ZIDSequence* ZEngine::IDSequence() {
  return idGenerator_;
}

ZGOFactory* ZEngine::GameObjectFactory() {
  return gameObjectFactory_;
}

ZGraphicsFactory* ZEngine::GraphicsFactory() {
  return graphicsFactory_;
}

float ZEngine::DeltaTime() {
  return deltaTime_;
}

float ZEngine::MilliSecondTime() {
  using namespace std::chrono;
  return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() / 1000.0f;
}

void ZEngine::Provide(ZGraphics* graphics) {
  if (graphics_ != nullptr) {
    delete graphics_;
  }
  graphics_ = graphics;
  graphics_->Initialize();
}

void ZEngine::Provide(ZDomain* domain) {
  if (domain_ != nullptr) {
    delete domain_;
  }
  domain_ = domain;
  domain_->Initialize();
}

void ZEngine::Provide(ZInput* input) {
  // If the provided input object is not null and the existing engine input object
  // is null, delete the existing one
  if (!dynamic_cast<ZNullInput*>(input) && dynamic_cast<ZNullInput*>(input_)) {
    delete input_;
  }
  input_ = input;
}

void ZEngine::Provide(ZUI* ui) {
  if (ui_ != nullptr) {
    delete ui_;
  }
  ui_ = ui;
  ui_->Initialize();
}

void ZEngine::Provide(ZPhysics* physics) {
  if (physics_ != nullptr) {
    delete physics_;
  }
  physics_ = physics;
  physics_->Initialize();
}

void ZEngine::SetDeltaTime(float deltaTime) {
  deltaTime_ = deltaTime;
}

ZGameObjectMap ZEngine::LoadZOF(std::string zofPath) {
  if (gameObjectFactory_ == nullptr) {
    gameObjectFactory_ = new ZGOFactory;
  }

  if (graphicsFactory_ == nullptr) {
    graphicsFactory_ = new ZGraphicsFactory;
  }

  // TODO: ZOFTree should have append functionality so that different
  // ZOFTrees can be combined (by combining children into a single tree)
  ZOFParser parser;
  ZOFTree* objectTree = parser.Parse(zofPath);

  // TODO: Move these two loops to a ZGraphics::Load() function
  ZShaderMap shaders = graphicsFactory_->CreateShaders(objectTree);
  for (ZShaderMap::iterator it = shaders.begin(); it != shaders.end(); it++) {
    if (graphics_ != nullptr) graphics_->AddShader(it->first, it->second);
  }

  ZTextureMap textures = graphicsFactory_->CreateTextures(objectTree);
  for (ZTextureMap::iterator it = textures.begin(); it != textures.end(); it++) {
    if (graphics_ != nullptr) graphics_->AddTexture(it->first, it->second);
  }

  return gameObjectFactory_->Create(objectTree);
}
