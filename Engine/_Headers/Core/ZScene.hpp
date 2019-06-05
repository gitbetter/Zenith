/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
	 ZScene.hpp
 
	 Created by Adrian Sanchez on 19/04/2019.
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

#pragma once

// Includes
#include "ZProcess.hpp"

// Forward Declarations
class ZLight;
class ZSkybox;
class ZUIElement;

// Class and Data Structure Definitions
class ZScene : public ZProcess, public std::enable_shared_from_this<ZScene> {
    
    friend class ZGraphics;
    
private:

	struct {
		std::mutex matrixStack;
		std::mutex pendingObjects;
	} sceneMutexes_;
    
	std::map<std::string, bool> pendingSceneDefinitions_;
    std::vector<std::string> pendingSceneObjects_;
	unsigned int loadedResourceCount_;

    
    // TODO: Create a light manager class to handle the scene lights
    std::shared_ptr<ZSkybox> skybox_ = nullptr;
    std::shared_ptr<ZGameObject> root_ = nullptr;
    std::shared_ptr<ZGameObject> activeCamera_ = nullptr;
	std::shared_ptr<ZGameObject> primaryCamera_ = nullptr;
	std::list<glm::mat4> matrixStack_;
    
    ZLightMap gameLights_;
    ZGameObjectMap gameObjects_;
    ZUIElementMap uiElements_;
    
	glm::mat4 viewProjection_, previousViewProjection_;
    ZPlayState playState_;
	std::string name_;
   
    void Render();
    void RenderUI();
	void LoadSceneData(std::shared_ptr<ZOFTree> objectTree);
    void ParseSceneMetadata(std::shared_ptr<ZOFTree> objectTree);
    void CreateSceneRoot(std::string &name);
	void UpdateViewProjectionMatrices();
	void UnregisterLoadDelegates();

	void HandleZOFReady(std::shared_ptr<ZEvent> event);
	void HandleTextureReady(std::shared_ptr<ZEvent> event);
	void HandleShaderReady(std::shared_ptr<ZEvent> event);
	void HandleModelReady(std::shared_ptr<ZEvent> event);
	void HandleSkyboxReady(std::shared_ptr<ZEvent> event);
	void HandleObjectDestroyed(std::shared_ptr<ZEvent> event);

	void CheckPendingObject(std::string type, std::shared_ptr<ZEvent>& event);
    
public:
    
    void extracted(std::string &name);
    
    ZScene(std::string name = "");
	ZScene(std::initializer_list<std::string> zofPaths);
    ~ZScene() { }
    
    void Initialize() override;
    void Update() override;
    
	void Play();
	void Pause() override;
	void Stop();

    std::shared_ptr<ZGameObject> Root() { return root_; }
    std::shared_ptr<ZSkybox> Skybox() { return skybox_; }
    std::shared_ptr<ZGameObject> ActiveCamera() { return activeCamera_; }
	std::shared_ptr<ZGameObject> PrimaryCamera() { return primaryCamera_; }
    ZGameObjectMap& GameObjects() { return gameObjects_; }
    ZUIElementMap& UIElements() { return uiElements_; }
    ZLightMap& GameLights() { return gameLights_; }
	std::string& Name() { return name_; }
	glm::mat4& ViewProjection() { return viewProjection_; }
	glm::mat4& PreviousViewProjection() { return previousViewProjection_; }
    ZPlayState& PlayState() { return playState_; }
    
    ZSceneSnapshot Snapshot();
    void RestoreSnapshot(ZSceneSnapshot& snapshot);
    
    void AddGameObject(std::shared_ptr<ZGameObject> gameObject, bool runImmediately = true);
    void AddGameObjects(std::initializer_list<std::shared_ptr<ZGameObject>> gameObjects, bool runImmediately = true);

	void RemoveGameObject(std::shared_ptr<ZGameObject> gameObject);
    
    void AddUIElement(std::shared_ptr<ZUIElement> element);
    void AddUIElements(std::initializer_list<std::shared_ptr<ZUIElement>> elements);

	glm::mat4 TopMatrix();
	void PushMatrix(glm::mat4 matrix);
	void PopMatrix();
    
	void SetPrimaryCamera(std::shared_ptr<ZGameObject> gameObject);
    void SetActiveCamera(std::shared_ptr<ZGameObject> gameObject);
    void SetSkybox(std::shared_ptr<ZSkybox> skybox) { skybox_ = skybox; }
    void SetDefaultSkybox();

	void CleanUp() override;
    
    void HandleQuit(std::shared_ptr<ZEvent> event);
};
