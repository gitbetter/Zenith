/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZGame.cpp

    Created by Adrian Sanchez on 1/27/19.
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

#include "ZServices.hpp"
#include "ZScene.hpp"
#include "ZGame.hpp"
#include "ZQuitEvent.hpp"
#include "ZDomain.hpp"
#include "ZInput.hpp"

using namespace std;

ZGame::ZGame(const std::string& name) : ZBase(), activeScene_(0), name_(name) {}

void ZGame::Provide(const std::shared_ptr<ZDomain>& domain)
{
    ZBase::Provide(domain);
    UpdateSceneSystems();
}

void ZGame::Provide(const std::shared_ptr<ZPhysicsUniverse>& physics)
{
    ZBase::Provide(physics);
    UpdateSceneSystems();
}

void ZGame::Provide(const std::shared_ptr<ZAudio>& audio)
{
    ZBase::Provide(audio);
    UpdateSceneSystems();
}

void ZGame::Provide(const std::shared_ptr<ZAssetStore>& assetStore)
{
    ZBase::Provide(assetStore);
    UpdateSceneSystems();
}

void ZGame::Configure(const ZGameOptions& options)
{
    ZBase::Configure(options);
    UpdateSceneConfig();
}

void ZGame::Setup()
{
    ZServices::EventAgent()->Subscribe(this, &ZGame::HandleQuit);
}

void ZGame::UpdateSceneSystems()
{
    for (auto scene : scenes_) {
        scene->SetGameSystems(gameSystems_);
    }
}

void ZGame::UpdateSceneConfig()
{
    for (auto scene : scenes_) {
        scene->SetGameConfig(gameOptions_);
    }
}

void ZGame::CleanUp()
{
    ZServices::EventAgent()->Unsubscribe(this, &ZGame::HandleQuit);
    ZBase::CleanUp();
}

void ZGame::Loop()
{
    LOG("Zenith is about to loop...", ZSeverity::Info);

    while (Running())
    {
        Tick();

        ZServices::Input()->PollEvents();
    }
}

void ZGame::Tick()
{
    if (previousTime_ == 0.0)
        previousTime_ = SECONDS_TIME;

    double currentTime = SECONDS_TIME;
    deltaTime_ = currentTime - previousTime_;
    previousTime_ = currentTime;

    ZServices::ProcessRunner()->UpdateTick(deltaTime_);

    if (onUpdateTickCallback_)
        onUpdateTickCallback_();

    if (!gameOptions_.domain.offline)
        Domain()->SwapBuffers();
}

bool ZGame::Running()
{
    return !Domain()->IsClosing();
}

void ZGame::AddScene(const std::shared_ptr<ZScene>& scene)
{
    activeScene_ = scenes_.size();
    scenes_.push_back(scene);
    scene->SetGameSystems(gameSystems_);
    scene->SetGameConfig(gameOptions_);
    ZServices::ProcessRunner()->AttachProcess(scene, ZPriority::High);
}

void ZGame::RemoveScene(const std::shared_ptr<ZScene>& scene)
{
    auto it = std::find(scenes_.begin(), scenes_.end(), scene);
    if (it != scenes_.end())
    {
        scenes_.erase(it);
        scene->SetGameSystems(ZGameSystems());
    }
}

void ZGame::SetActiveScene(unsigned int index)
{
    if (index < 0 || index >= scenes_.size() || index == activeScene_) return;
    if (!scenes_.empty() && scenes_[activeScene_]->IsAlive())
        scenes_[activeScene_]->Abort();
    activeScene_ = index;
}

void ZGame::HandleQuit(const std::shared_ptr<ZQuitEvent>& event)
{
    ZServices::Input()->ReleaseCursor();
}

std::shared_ptr<ZGame> ZGame::Create(const std::string& name)
{
    return std::make_shared<ZGame>(name);
}
