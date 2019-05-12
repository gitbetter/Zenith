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
#include "ZUICursor.hpp"
#include "ZBulletPhysics.hpp"
#include "ZALAudio.hpp"
#include "ZIDSequence.hpp"
#include "ZOFTree.hpp"
#include "ZOFParser.hpp"
#include "ZProcessRunner.hpp"
#include "ZResourceCache.hpp"
#include "ZScriptResourceLoader.hpp"
#include "ZWavResourceLoader.hpp"
#include "ZOggResourceLoader.hpp"
#include "ZEventAgent.hpp"
#include "ZGOFactory.hpp"
#include "ZGraphicsFactory.hpp"
#include "ZUIFactory.hpp"
#include "ZPhysicsFactory.hpp"
#include "ZScriptExports.hpp"
#include "ZLuaScriptManager.hpp"
#include "ZScriptableProcess.hpp"

#ifdef DEV_BUILD
#include "ZDevResourceFile.hpp"
#else
#include "ZZipFile.hpp"
#endif

const float ZEngine::DEFAULT_X_RESOLUTION = 2560.f;
const float ZEngine::DEFAULT_Y_RESOLUTION = 1600.f;
const float ZEngine::UPDATE_STEP_SIZE = 0.017f;
const int ZEngine::MAX_FIXED_UPDATE_ITERATIONS = 50;
const unsigned int ZEngine::SHADOW_MAP_SIZE = 4096;
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
std::shared_ptr<ZAudio> ZEngine::audio_ = nullptr;
std::unique_ptr<ZProcessRunner> ZEngine::processRunner_ = nullptr;
std::shared_ptr<ZEventAgent> ZEngine::eventAgent_ = nullptr;
std::unique_ptr<ZResourceCache> ZEngine::resourceCache_ = nullptr;
std::unique_ptr<ZLuaScriptManager> ZEngine::scriptManager_ = nullptr;
std::unique_ptr<ZGOFactory> ZEngine::gameObjectFactory_ = nullptr;
std::unique_ptr<ZGraphicsFactory> ZEngine::graphicsFactory_ = nullptr;
std::unique_ptr<ZUIFactory> ZEngine::uiFactory_ = nullptr;
std::unique_ptr<ZPhysicsFactory> ZEngine::physicsFactory_ = nullptr;

std::unique_ptr<ZIDSequence> ZEngine::idGenerator_(new ZIDSequence);
double ZEngine::deltaTime_ = 0.0;
double ZEngine::lastDeltaTime_ = 0.0;

// TODO: Useful to have a config file to parse for more global state info such as window dimensions
// and maximum resource cache size
void ZEngine::Initialize(std::shared_ptr<ZGame> game, int windowWidth, int windowHeight) {
    currentGame_ = game;
    
    /* ========= Process System ============ */
    processRunner_.reset(new ZProcessRunner);
    /* ===================================== */
    
    /* ========= Resource Cache System ============ */
    resourceCache_.reset(new ZResourceCache(100));
#ifdef DEV_BUILD
    resourceCache_->RegisterResourceFile(std::shared_ptr<ZDevResourceFile>(new ZDevResourceFile("../Assets")));
#else
    resourceCache_->RegisterResourceFile(std::shared_ptr<ZZipFile>(new ZZipFile("Assets.zip")));
#endif
    resourceCache_->Initialize();
    resourceCache_->RegisterLoader(std::shared_ptr<ZScriptResourceLoader>(new ZScriptResourceLoader));
    resourceCache_->RegisterLoader(std::shared_ptr<ZWavResourceLoader>(new ZWavResourceLoader));
    resourceCache_->RegisterLoader(std::shared_ptr<ZOggResourceLoader>(new ZOggResourceLoader));
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
    physics_ = std::make_shared<ZBulletPhysics>();
    physics_->Initialize();
    processRunner_->AttachProcess(physics_);
    /* ===================================== */
    
    /* ========= Audio System ============ */
    audio_ = std::make_shared<ZALAudio>();
    audio_->Initialize();
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

ZAudio* ZEngine::Audio() {
    return audio_.get();
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

double ZEngine::LastDeltaTime() {
    return lastDeltaTime_;
}

double ZEngine::DeltaTime() {
    return deltaTime_;
}

double ZEngine::SecondsTime() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() / 1000.0;
}

void ZEngine::Provide(std::shared_ptr<ZDomain> domain) {
    domain_ = domain;
    domain_->Initialize();
}

void ZEngine::Provide(std::shared_ptr<ZGraphics> graphics) {
    graphics_ = graphics;
    graphics_->Initialize();
}

void ZEngine::Provide(std::shared_ptr<ZInput> input) {
    if(input_) input_->Abort();
    input_ = input;
    processRunner_->AttachProcess(input_);
}

void ZEngine::Provide(std::shared_ptr<ZUI> ui) {
    ui_ = ui;
    ui_->Initialize();
}

void ZEngine::Provide(std::shared_ptr<ZPhysics> physics) {
    if (physics_) physics_->Abort();
    physics_ = physics;
    physics_->Initialize();
}

void ZEngine::Provide(std::shared_ptr<ZAudio> audio) {
    audio_ = audio;
    audio_->Initialize();
}

void ZEngine::SetDeltaTime(double deltaTime) {
    lastDeltaTime_ = deltaTime_;
    deltaTime_ = deltaTime;
}

std::shared_ptr<ZOFTree> ZEngine::LoadZOF(std::string zofPath) {
    ZResource zofResource(zofPath);
    std::shared_ptr<ZResourceHandle> handle = resourceCache_->GetHandle(&zofResource);
    
    if (!handle) {
        _Z("Could not find zof file at " + zofPath, ZERROR);
		return std::shared_ptr<ZOFTree>();
    }
    
    // TODO: ZOFTree should have append functionality so that different
    // ZOFTrees can be combined (by combining children into a single tree)
    ZOFParser parser;
    std::shared_ptr<ZOFTree> objectTree = parser.Parse(std::string((char*)handle->Buffer()));
    
	return objectTree;
}

std::shared_ptr<ZScene> ZEngine::LoadScene(std::initializer_list<std::string> zofPaths) {
	std::shared_ptr<ZScene> scene = std::make_shared<ZScene>();

	// Parse each ZOF file and create the resources
	for (std::string path : zofPaths) {
		std::shared_ptr<ZOFTree> objectTree = LoadZOF(path);

		// TODO: The more systems are populated this way, the more of a hamper we place on
		// load times. Refactor this so the object tree is only traversed once.
		if (scriptManager_ != nullptr) scriptManager_->Load(objectTree);
		if (graphics_ != nullptr) graphics_->Load(objectTree); // Slow

		ZOFLoadResult zofResults;
		zofResults.gameObjects = gameObjectFactory_->Load(objectTree); // Slow
		zofResults.uiElements = uiFactory_->Load(objectTree);

		for (ZGameObjectMap::iterator it = zofResults.gameObjects.begin(); it != zofResults.gameObjects.end(); it++) {
			scene->AddGameObject(it->second);
		}

		for (ZUIElementMap::iterator it = zofResults.uiElements.begin(); it != zofResults.uiElements.end(); it++) {
			if (std::dynamic_pointer_cast<ZUICursor>(it->second))
				ui_->SetCursor(std::dynamic_pointer_cast<ZUICursor>(it->second));
			else
				ui_->AddElement(it->second);
		}
	}

	// Set a default skybox for our scene
	// TODO: Only set this if no skybox is set after loading the scene descriptions
	scene->SetDefaultSkybox(); // Slow

	currentGame_->AddScene(scene);

	return scene;
}

void ZEngine::CleanUp() {
    processRunner_->AbortAllProcesses(true);
    
    audio_->CleanUp(); audio_.reset();
    physics_->CleanUp(); physics_.reset();
    ui_->CleanUp(); ui_.reset();
    input_.reset();
    graphics_->CleanUp(); graphics_.reset();
    domain_->CleanUp(); domain_.reset();
    resourceCache_.reset();
    eventAgent_->CleanUp();
    scriptManager_.reset();
}
