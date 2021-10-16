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

#include "ZProcess.hpp"
#include "ZOFTree.hpp"
#include "ZBVH.hpp"

class ZGame;
class ZLight;
class ZSkybox;
class ZCamera;
class ZUIElement;
class ZRenderer;
class ZUICanvas;
class ZRaycastEvent;
class ZFramebuffer;
class ZObjectDestroyedEvent;
class ZResourceLoadedEvent;
class ZWindowResizeEvent;
class ZRay;
class ZBVH;
class ZAssetLoadProgressTracker;

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

    ZHGameObject Root() { return root_; }
    ZHGameObject Skybox() { return skybox_; }
    ZHGameObject ActiveCamera() { return activeCamera_; }
    ZHGameObject PrimaryCamera() { return primaryCamera_; }
    ZGameObjectList& GameObjects() { return gameObjects_; }
    ZUIElementList& UIElements() { return uiElements_; }
    ZLightList& GameLights() { return gameLights_; }
    std::string& Name() { return name_; }
    ZPlayState& PlayState() { return playState_; }
    ZHTexture TargetTexture();
    std::shared_ptr<ZPhysicsUniverse> PhysicsUniverse() const { return gameSystems_.physics; }
    std::shared_ptr<ZDomain> Domain() const { return gameSystems_.domain; }
    std::shared_ptr<ZAudio> Audio() const { return gameSystems_.audio; }
    ZGameOptions GameConfig() { return gameConfig_; }
    std::string GameName() const { return gameName_; }
    ZFrameStats& FrameStats() { return frameStats_; }

    void SetGameSystems(const ZGameSystems& systems) { gameSystems_ = systems; }
    void SetGameConfig(const ZGameOptions& options) { gameConfig_ = options; }
    void SetGameName(const std::string& name) { gameName_ = name; }
    void SetPrimaryCamera(const ZHGameObject& camera) { primaryCamera_ = camera; }
    void SetActiveCamera(const ZHGameObject& camera) { activeCamera_ = camera; }
    void SetSkybox(const ZHGameObject& skybox) { skybox_ = skybox; }
    void SetDefaultSkybox();

    ZSceneSnapshot Snapshot();
    void RestoreSnapshot(ZSceneSnapshot& snapshot);

    void AddBVHPrimitive(const ZBVHPrimitive& primitive);

    void AddGameObject(const ZHGameObject& gameObject, bool runImmediately = true);
    void AddGameObjects(std::initializer_list<ZHGameObject> gameObjects, bool runImmediately = true);
    ZHGameObject FindGameObjectByName(const std::string& name);
    void RemoveGameObject(const ZHGameObject& gameObject);

    void AddUIElement(const ZHUIElement& element);
    void AddUIElements(std::initializer_list<ZHUIElement> elements);
    ZHUIElement FindUIElement(const std::string& name);
    void RemoveUIElement(const ZHUIElement& element);

    ZRay ScreenPointToWorldRay(const glm::vec2& point, const glm::vec2& dimensions = glm::vec2(0.f));
    bool RayCast(ZRay& ray, ZIntersectHitResult& hitResult);

    void UpdateLightspaceMatrices();

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
        std::mutex pendingObjects;
    } sceneMutexes_;

    std::map<std::string, bool>                 pendingSceneDefinitions_;
    std::vector<std::string>                    pendingSceneObjects_;

    std::string                                 name_;
    std::string                                 gameName_;
    ZPlayState                                  playState_;

    std::shared_ptr<ZRenderer>                  renderer_ = nullptr;
    std::shared_ptr<ZFramebuffer>               targetBuffer_ = nullptr;

    // TODO: Create a light manager class to handle the scene lights
    ZHGameObject                                skybox_;
    ZHGameObject                                root_;
    ZHUIElement                                 canvas_;
    ZHGameObject                                activeCamera_;
    ZHGameObject                                primaryCamera_;
    std::shared_ptr<ZBVH>                       bvh_ = nullptr;
    std::shared_ptr<ZAssetLoadProgressTracker>  loadProgressTracker_ = nullptr;

    ZIDMap                                      gameLightIDMap_;
    ZLightList                                  gameLights_;
    ZIDMap                                      gameObjectIDMap_;
    ZGameObjectList                             gameObjects_;
    ZIDMap                                      uiElementIDMap_;
    ZUIElementList                              uiElements_;

    ZGameSystems                                gameSystems_;
    ZGameOptions                                gameConfig_;
    ZFrameStats                                 frameStats_;

protected:

    void SetupRenderers();
    void SetupTargetDrawBuffer();
    void SetupRenderPasses();
    void LoadSceneData(const std::shared_ptr<ZOFNode>& objectTree);
    void ParseSceneMetadata(const std::shared_ptr<ZOFNode>& objectTree);
    void CreateSceneRoot(const std::string& name);
    void CreateUICanvas();
    void UnregisterLoadDelegates();

    void HandleWindowResize(const std::shared_ptr<ZWindowResizeEvent>& event);
    void HandleZOFReady(const std::shared_ptr<ZResourceLoadedEvent>& event);
    void HandleObjectDestroyed(const std::shared_ptr<ZObjectDestroyedEvent>& event);
    void HandleRaycastEvent(const std::shared_ptr<ZRaycastEvent>& event);

};
