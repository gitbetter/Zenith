/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZEditor.hpp
 
    Created by Adrian Sanchez on 18/01/21.
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
#include "ZGame.hpp"

class ZEditorScene;
class ZSceneReadyEvent;

class ZEditor : public ZGame, public std::enable_shared_from_this<ZEditor> {
    
public:
    
    ZEditor() : ZGame("Editor") { }
    
    void CleanUp() override;

protected:

    std::shared_ptr<ZGame>                      project_ = nullptr;
    std::shared_ptr<ZEditorScene>               editorScene_ = nullptr;
	ZGameObjectMap                              gameObjectTemplates_;
	ZUIElementMap                               uiElementTemplates_;

    void Setup() override;

    void CreateDefaultProject();

    void HandleSceneLoaded(const std::shared_ptr<ZSceneReadyEvent>& event);
    
};
