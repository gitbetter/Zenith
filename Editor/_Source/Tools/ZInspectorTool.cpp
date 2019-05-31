/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZInspectorTool.cpp
 
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

#include "ZInspectorTool.hpp"
#include "ZEditor.hpp"
#include "ZGameObject.hpp"
#include "ZGraphics.hpp"
#include "ZGraphicsStrategy.hpp"
#include "ZTextureReadyEvent.hpp"
#include "ZEventAgent.hpp"

void ZInspectorTool::Begin() {
	ImGui::Begin(name_.c_str());
}

void ZInspectorTool::Initialize() {
    ZEventDelegate resourceLoadedDelegate = fastdelegate::MakeDelegate(this, &ZInspectorTool::HandleTextureLoaded);
    zenith::EventAgent()->AddListener(resourceLoadedDelegate, ZTextureReadyEvent::Type);
    
    zenith::Graphics()->Strategy()->LoadTextureAsync(OBJECT_CUBE_IMAGE_PATH, "");
    memset(objectNameBuffer_, 0, 512);
}

void ZInspectorTool::Update() {
    std::shared_ptr<ZGameObject> selectedObject;
    if (editor_->SelectedObjects().size() == 1)
        selectedObject = editor_->SelectedObjects().begin()->second;
    
    if (selectedObject) {
        if (strcmp(objectNameBuffer_, selectedObject->Name().c_str()) != 0) {
            if (selectedObject->Name().empty())
                memset(objectNameBuffer_, 0, 512);
            else
                memcpy(objectNameBuffer_, selectedObject->Name().c_str(), selectedObject->Name().size());
        }
    }
    
    ImGui::Image((ImTextureID)objectCubeImage_.id, ImVec2(40.0, 40.0));
    
    ImGui::SameLine();
    
    ImGui::SetCursorPos(ImVec2(60.0, 37.0));
    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, selectedObject == nullptr);
    if (ImGui::InputText("##name", objectNameBuffer_, 512, ImGuiInputTextFlags_EnterReturnsTrue)) {
        if (selectedObject) {
            std::string name(objectNameBuffer_);
            selectedObject->SetName(name);
        }
    }
    ImGui::PopItemFlag();
}

void ZInspectorTool::End() {
	ImGui::End();
}

void ZInspectorTool::HandleTextureLoaded(std::shared_ptr<ZEvent> event) {
    std::shared_ptr<ZTextureReadyEvent> textureEvent = std::dynamic_pointer_cast<ZTextureReadyEvent>(event);
    if (textureEvent->Texture().path == OBJECT_CUBE_IMAGE_PATH) {
        objectCubeImage_ = textureEvent->Texture();
        
        ZEventDelegate resourceLoadedDelegate = fastdelegate::MakeDelegate(this, &ZInspectorTool::HandleTextureLoaded);
        zenith::EventAgent()->RemoveListener(resourceLoadedDelegate, ZTextureReadyEvent::Type);
    }
}
