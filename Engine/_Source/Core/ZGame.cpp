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

#include "ZGame.hpp"
#include "ZDomain.hpp"
#include "ZGraphics.hpp"
#include "ZGraphicsStrategy.hpp"
#include "ZProcessRunner.hpp"

#include <chrono>
using namespace std;

ZGame::ZGame() : activeScene_(0) {}

void ZGame::RunGameLoop()
{
    zenith::Log("Zenith is about to loop...", ZSeverity::Info);

    double previousTime = zenith::SecondsTime();

    while (Running())
    {
        double currentTime = zenith::SecondsTime();
        zenith::SetDeltaTime(currentTime - previousTime);
        previousTime = currentTime;

        zenith::ProcessRunner()->UpdateTick();

        zenith::Graphics()->Strategy()->SwapBuffers();

        zenith::Domain()->Strategy()->PollEvents();
    }
}

bool ZGame::Running()
{
    return !zenith::Domain()->Strategy()->IsWindowClosing();
}

void ZGame::AddScene(std::shared_ptr<ZScene> scene)
{
    activeScene_ = scenes_.size();
    scenes_.push_back(scene);
    zenith::ProcessRunner()->AttachProcess(scene, ZPriority::High);
}

void ZGame::RemoveScene(std::shared_ptr<ZScene> scene)
{
    auto it = std::find(scenes_.begin(), scenes_.end(), scene);
    if (it != scenes_.end())
    {
        scenes_.erase(it);
    }
}

void ZGame::SetActiveScene(unsigned int index)
{
    if (index < 0 || index >= scenes_.size() || index == activeScene_) return;
    if (!scenes_.empty() && scenes_[activeScene_]->IsAlive())
        scenes_[activeScene_]->Abort();
    activeScene_ = index;
}
