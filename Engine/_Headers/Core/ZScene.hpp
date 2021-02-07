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
class ZGame;
class ZLight;
class ZSkybox;
class ZCamera;
class ZUIElement;
class ZRenderer3D;
class ZRenderer2D;
class ZUICanvas;
class ZRaycastEvent;
class ZFramebuffer;
class ZObjectDestroyedEvent;
class ZResourceLoadedEvent;
class ZWindowResizeEvent;
class ZTextureReadyEvent;
class ZShaderReadyEvent;
class ZModelReadyEvent;
class ZSkyboxReadyEvent;

// Class and Data Structure Definitions
class ZScene : public ZProcess, public std::enable_shared_from_this<ZScene>
{

    friend class ZGraphics;

public:

    ZScene(const std::string& name = "");
    ZScene(std::initializer_list<std::string> zofPaths);
    virtual ~ZScene() {}

    virtual void Initialize() override;
    virtual void Update(double deltaTime) override;
    virtual void CleanUp() override;
    virtual void Play();
    virtual void Pause() override;
    virtual void Stop();
    virtual void Finish() override;

    std::shared_ptr<ZGameObject> Root() { return root_; }
    std::shared_ptr<ZSkybox> Skybox() { return skybox_; }
    std::shared_ptr<ZCamera> ActiveCamera() { return activeCamera_; }
    std::shared_ptr<ZCamera> PrimaryCamera() { return primaryCamera_; }
    ZGameObjectMap& GameObjects() { return gameObjects_; }
    ZUIElementMap& UIElements() { return uiElements_; }
    ZLightMap& GameLights() { return gameLights_; }
    std::string& Name() { return name_; }
    glm::mat4& ViewProjection() { return viewProjection_; }
    glm::mat4& PreviousViewProjection() { return previousViewProjection_; }
    ZPlayState& PlayState() { return playState_; }
    std::shared_ptr<ZTexture> TargetTexture();
    std::shared_ptr<ZPhysicsUniverse> PhysicsUniverse() const { return gameSystems_.physics; }
    std::shared_ptr<ZDomain> Domain() const { return gameSystems_.domain; }
    std::shared_ptr<ZAudio> Audio() const { return gameSystems_.audio; }
    std::shared_ptr<ZAssetStore> AssetStore() const { return gameSystems_.assetStore; }
    ZGameOptions GameConfig() { return gameConfig_; }

    void SetGameSystems(const ZGameSystems& systems) { gameSystems_ = systems; }
    void SetGameConfig(const ZGameOptions& options) { gameConfig_ = options; }
    void SetPrimaryCamera(std::shared_ptr<ZCamera> camera) { primaryCamera_ = camera; }
    void SetActiveCamera(std::shared_ptr<ZCamera> camera) { activeCamera_ = camera; }
    void SetSkybox(std::shared_ptr<ZSkybox> skybox) { skybox_ = skybox; }
    void SetDefaultSkybox();

    ZSceneSnapshot Snapshot();
    void RestoreSnapshot(ZSceneSnapshot& snapshot);

    void AddGameObject(std::shared_ptr<ZGameObject> gameObject, bool runImmediately = true);
    void AddGameObjects(std::initializer_list<std::shared_ptr<ZGameObject>> gameObjects, bool runImmediately = true);
    std::shared_ptr<ZGameObject> FindGameObject(const std::string& id);
    void RemoveGameObject(std::shared_ptr<ZGameObject> gameObject);

    void AddUIElement(std::shared_ptr<ZUIElement> element);
    void AddUIElements(std::initializer_list<std::shared_ptr<ZUIElement>> elements);
    std::shared_ptr<ZUIElement> FindUIElement(const std::string& id);
    void RemoveUIElement(std::shared_ptr<ZUIElement> element);

    glm::mat4 TopMatrix();
    void PushMatrix(const glm::mat4& matrix);
    void PopMatrix();

    template <class T, typename... Args>
    static std::shared_ptr<T> Load(Args&&... args)
    {
        if (!std::is_base_of<ZScene, T>::value)
        {
            LOG("Cannot load " + std::string(typeid(T).name()) + " as a ZScene object", ZSeverity::Error);
            return nullptr;
        }
        std::shared_ptr<T> scene = std::make_shared<T>(std::forward<Args>(args)...);
        return scene;
    }

    template <class T, typename... Args>
    static std::shared_ptr<T> LoadIn(const std::shared_ptr<ZGame>& game, Args&&... args)
    {
        if (!std::is_base_of<ZScene, T>::value)
        {
            LOG("Cannot load " + std::string(typeid(T).name()) + " as a ZScene object", ZSeverity::Error);
            return nullptr;
        }
        std::shared_ptr<T> scene = std::make_shared<T>(std::forward<Args>(args)...);
        game->AddScene(scene);
        return scene;
    }

protected:

    struct
    {
        std::mutex matrixStack;
        std::mutex pendingObjects;
    } sceneMutexes_;

    std::map<std::string, bool> pendingSceneDefinitions_;
    std::vector<std::string> pendingSceneObjects_;

    std::string name_;
    ZPlayState playState_;

    std::shared_ptr<ZRenderer3D> renderer3D_ = nullptr;
    std::shared_ptr<ZRenderer2D> renderer2D_ = nullptr;
    std::shared_ptr<ZFramebuffer> targetBuffer_ = nullptr;

    // TODO: Create a light manager class to handle the scene lights
    std::shared_ptr<ZSkybox> skybox_ = nullptr;
    std::shared_ptr<ZGameObject> root_ = nullptr;
    std::shared_ptr<ZUICanvas> canvas_ = nullptr;
    std::shared_ptr<ZCamera> activeCamera_ = nullptr;
    std::shared_ptr<ZCamera> primaryCamera_ = nullptr;

    ZLightMap gameLights_;
    ZGameObjectMap gameObjects_;
    ZUIElementMap uiElements_;

    ZGameSystems gameSystems_;
    ZGameOptions gameConfig_;

    glm::mat4 viewProjection_, previousViewProjection_;
    std::list<glm::mat4> matrixStack_;

    void SetupRenderers();
    void SetupTargetDrawBuffer();
    void SetupRenderPasses();
    void LoadSceneData(const std::shared_ptr<ZOFTree>& objectTree);
    void ParseSceneMetadata(const std::shared_ptr<ZOFTree>& objectTree);
    void CheckPendingObjects();
    void CreateSceneRoot(const std::string& name);
    void CreateUICanvas();
    void UpdateViewProjectionMatrices();
    void UpdateLightspaceMatrices();
    void UnregisterLoadDelegates();

    void HandleWindowResize(const std::shared_ptr<ZWindowResizeEvent>& event);
    void HandleZOFReady(const std::shared_ptr<ZResourceLoadedEvent>& event);
    void HandleTextureReady(const std::shared_ptr<ZTextureReadyEvent>& event);
    void HandleShaderReady(const std::shared_ptr<ZShaderReadyEvent>& event);
    void HandleModelReady(const std::shared_ptr<ZModelReadyEvent>& event);
    void HandleSkyboxReady(const std::shared_ptr<ZSkyboxReadyEvent>& event);
    void HandleObjectDestroyed(const std::shared_ptr<ZObjectDestroyedEvent>& event);
    void HandleRaycastEvent(const std::shared_ptr<ZRaycastEvent>& event);

};
