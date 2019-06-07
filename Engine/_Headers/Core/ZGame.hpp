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
#include "ZScene.hpp"

// Forward Declarations

// Class and Data Structure Definitions
class ZGame {
    
private:
    
    // TODO: Store this information in a scene manager class
    unsigned int activeScene_ = 0;
    std::vector<std::shared_ptr<ZScene>> scenes_;
    
public:
    
    ZGame();
    ~ZGame() { };
    
    void CleanUp() { }
    
    std::shared_ptr<ZScene> ActiveScene() { return !(scenes_.empty()) ? scenes_[activeScene_] : nullptr; }
    void SetActiveScene(unsigned int index);
    
    void RunGameLoop();
    bool Running();
    void AddScene(std::shared_ptr<ZScene> scene);
    
};
