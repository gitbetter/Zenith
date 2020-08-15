/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZInspectorTool.hpp
 
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
class ZEvent;

// Definitions
const std::string OBJECT_CUBE_IMAGE_PATH = std::string(EDITOR_ROOT) + "/_Assets/Images/object_cube.png";

class ZInspectorTool : public ZEditorTool {
    
    friend class ZEditor;
    
private:

	float angleToRadsRatio_ = glm::pi<float>() / 180.f;
	float radsToAngleRatio_ = 180.f / glm::pi<float>();
    
    ZTexture objectCubeImage_;
    char objectNameBuffer_[512];
	std::string lastSelectedObject_;
    
	void DrawNameField(std::shared_ptr<ZGameObject> &selectedObject);
	void DrawTransformProperties(std::shared_ptr<ZGameObject> &selectedObject);
	void DrawComponentProperties(std::shared_ptr<ZGameObject> &selectedObject);
	void CaptureMouse();
	void ReleaseMouse();
	bool RedoButton(std::shared_ptr<ZGameObject>& selectedObject, const std::string& prop);
    void HandleTextureLoaded(std::shared_ptr<ZEvent> event);

public:

	ZInspectorTool() : ZEditorTool("Inspector") { }
    
    void Initialize() override;

	void Begin() override;
	void Update() override;
	void End() override;

};
