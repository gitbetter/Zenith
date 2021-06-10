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
#include "ZServices.hpp"
#include "ZScene.hpp"
#include "ZUIPanel.hpp"
#include "ZUIVerticalLayout.hpp"
#include "ZGameObjectControls.hpp"
#include "ZEditorObjectSelectedEvent.hpp"
#include "ZFoldoutRegion.hpp"

void ZInspectorTool::Initialize(const std::shared_ptr<ZScene>& scene) {
    ZEditorTool::Initialize(scene);
    container_->SetColor(theme_.secondaryColor);
    container_->SetPadding(glm::vec2(10.f, 10.f));

    ZUILayoutOptions layoutOptions;
    layoutOptions.itemSpacing = 10.f;
    container_->SetLayout(std::make_shared<ZUIVerticalLayout>(layoutOptions));

    gameObjectControls_ = ZGameObjectControls::Create(nullptr, scene, theme_);

    ZServices::EventAgent()->Subscribe(this, &ZInspectorTool::HandleEditorObjectSelected);
}

void ZInspectorTool::Update()
{
    if (gameObjectControls_->Active()) {
        gameObjectControls_->Update();
    }
}

void ZInspectorTool::CleanUp()
{
    if (gameObjectControls_) {
        gameObjectControls_->CleanUp();
    }
}

void ZInspectorTool::HandleEditorObjectSelected(const std::shared_ptr<ZEditorObjectSelectedEvent>& event)
{
    if (event->ObjectID().empty()) {
        selectedObject_ = nullptr;
        gameObjectControls_->SetGameObject(selectedObject_);
        container_->RemoveChild(gameObjectControls_->Header());
        container_->RemoveChild(gameObjectControls_->TransformFields()->Container());
    }
    else if (auto obj = activeProjectScene_->FindGameObject(event->ObjectID())) {
        selectedObject_ = obj;
        gameObjectControls_->SetGameObject(selectedObject_);
        container_->AddChild(gameObjectControls_->Header());
        container_->AddChild(gameObjectControls_->TransformFields()->Container());
    }
}