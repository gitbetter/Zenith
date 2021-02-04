/*

 ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
 /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
 \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

 ZGame.hpp

 Created by Adrian Sanchez on 27/01/2019.
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
#include "ZBase.hpp"

// Forward Declarations
class ZQuitEvent;
class ZScene;

// Class and Data Structure Definitions
class ZGame : public ZBase
{

protected:

    // TODO: Store this information in a scene manager class
    unsigned int activeScene_ = 0;
    std::vector<std::shared_ptr<ZScene>> scenes_;

    std::string name_;
    double previousTime_ = 0.0;
    double deltaTime_ = 0.0;
    std::function<void()> onUpdateTickCallback_;

public:

    ZGame(const std::string& name = "Untitled");
    virtual ~ZGame() {};

    void Provide(const std::shared_ptr<ZDomain>& domain) override;
    void Provide(const std::shared_ptr<ZPhysicsUniverse>& physics) override;
    void Provide(const std::shared_ptr<ZAudio>& audio) override;
    void Provide(const std::shared_ptr<ZAssetStore>& store) override;
    void Configure(const ZGameOptions& options) override;

    virtual void CleanUp() override;

    std::shared_ptr<ZScene> ActiveScene() { return !(scenes_.empty()) ? scenes_[activeScene_] : nullptr; }
    const std::string& Name() const { return name_; }

    void SetActiveScene(unsigned int index);
    void SetName(const std::string& name) { name_ = name; }

    void OnUpdateTick(const std::function<void()>& onTick) { onUpdateTickCallback_ = onTick; }

    void Loop();
    void Tick();
    bool Running();
    void AddScene(const std::shared_ptr<ZScene>& scene);
    void RemoveScene(const std::shared_ptr<ZScene>& scene);

    void HandleQuit(const std::shared_ptr<ZQuitEvent>& event);

    static std::shared_ptr<ZGame> Create(const std::string& name = "Untitled");

protected:

    virtual void Setup() override;

    void UpdateSceneSystems();
    void UpdateSceneConfig();

};
