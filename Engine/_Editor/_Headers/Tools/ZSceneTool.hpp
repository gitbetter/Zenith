/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZSceneTool.hpp
 
    Created by Adrian Sanchez on 18/05/19.
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
#include "ZEditorTool.hpp"

// Forward Declarations
class ZGameObject;
class ZUIImage;
class ZEditorGizmo;
class ZUIClicker;
class ZFrameStatsDisplay;
class ZGameObject;

// Definitions
class ZSceneTool : public ZEditorTool {
    
public:

	ZSceneTool(const ZUITheme& theme = ZUITheme())
        : ZEditorTool("Scene", theme) { }

    void Initialize(const std::shared_ptr<ZScene>& scene) override;

    void SetSelectedObject(const std::shared_ptr<ZGameObject>& object);
    
	void Update() override;

private:

    std::shared_ptr<ZEditorGizmo> currentGizmo_ = nullptr;
    std::vector<std::shared_ptr<ZEditorGizmo>> gizmos_;

    std::shared_ptr<ZUIClicker> selectClicker_ = nullptr;
    std::shared_ptr<ZUIClicker> travelClicker_ = nullptr;

    std::shared_ptr<ZGameObject> selectedObject_ = nullptr;

    void OnProjectSceneChanged() override;

    void SetupGizmos(const std::shared_ptr<ZScene>& scene);
    void AddGizmo(const std::shared_ptr<ZEditorGizmo>& gizmo, const std::shared_ptr<ZScene>& scene);

};
