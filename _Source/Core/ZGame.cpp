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
#include "ZEngine.hpp"
#include "ZDomain.hpp"
#include "ZProcessRunner.hpp"

#include <chrono>
using namespace std;

ZGame::ZGame() : activeScene_(0) { }

void ZGame::RunGameLoop() {
    if (scenes_.size() == 0) {
        _Z("Cannot kick off the game loop without any scenes present!", ZERROR);
        return;
    }
    
    _Z("Zenith is about to loop...", ZINFO);
    
    double previousTime = ZEngine::SecondsTime();
    
    while (!ZEngine::Domain()->Strategy()->IsWindowClosing()) {
		double currentTime = ZEngine::SecondsTime();
        ZEngine::SetDeltaTime(currentTime - previousTime);
        previousTime = currentTime;
        
        ZEngine::ProcessRunner()->UpdateProcesses();
        
        ZEngine::Domain()->Strategy()->PollEvents();
        
        MacDisplayHack();
    }
}

void ZGame::AddScene(std::shared_ptr<ZScene> scene) {
    activeScene_ = scenes_.size();
    scenes_.push_back(scene);
    ZEngine::ProcessRunner()->AttachProcess(scene);
}

void ZGame::SetActiveScene(unsigned int index) {
    if (index < 0 || index >= scenes_.size()) return;
    activeScene_ = index;
}

// -.-
void ZGame::MacDisplayHack() {
#ifdef __APPLE__
    static bool moved = false;
    if (!moved) {
        ZEngine::Domain()->ResizeWindow(
                                        ZEngine::Domain()->WindowWidth() - 1,
                                        ZEngine::Domain()->WindowHeight() - 1
                                        );
        moved = true;
    }
#endif
}
