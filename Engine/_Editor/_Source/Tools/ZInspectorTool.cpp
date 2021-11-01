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
#include "ZGameObject.hpp"
#include "ZComponent.hpp"
#include <rttr/type>

void ZInspectorTool::Initialize(const std::shared_ptr<ZScene>& scene)
{
    ZEditorTool::Initialize(scene);

    ZServices::UIElementManager()->SetColor(container_, theme_.secondaryColor);
    ZServices::UIElementManager()->SetPadding(container_, glm::vec2(10.f, 10.f));

    ZUILayoutOptions layoutOptions;
    layoutOptions.itemSpacing = 10.f;
    ZServices::UIElementManager()->SetLayout(container_, std::make_shared<ZUIVerticalLayout>(layoutOptions));

    gameObjectControls_ = ZGameObjectControls::Create(ZHGameObject(), scene, theme_);

    ZServices::EventAgent()->Subscribe(this, &ZInspectorTool::HandleEditorObjectSelected);
}

void ZInspectorTool::Update()
{
    if (gameObjectControls_->Active())
    {
        gameObjectControls_->Update();
    }
}

void ZInspectorTool::CleanUp()
{
    if (gameObjectControls_)
    {
        gameObjectControls_->CleanUp();
    }
}

void ZInspectorTool::HandleEditorObjectSelected(const std::shared_ptr<ZEditorObjectSelectedEvent>& event)
{
    if (event->ObjectID().empty())
    {
        selectedObject_ = ZHGameObject();
        gameObjectControls_->SetGameObject(selectedObject_);
        ZServices::UIElementManager()->RemoveChild(container_, gameObjectControls_->Header());
        ZServices::UIElementManager()->RemoveChild(container_, gameObjectControls_->TransformFields()->Container());
    }
    else if (auto obj = activeProjectScene_->FindGameObjectByName(event->ObjectID()))
    {
        selectedObject_ = obj;
        gameObjectControls_->SetGameObject(selectedObject_);
        ZServices::UIElementManager()->AddChild(container_, gameObjectControls_->Header());
        ZServices::UIElementManager()->AddChild(container_, gameObjectControls_->TransformFields()->Container());

        for (const auto& comp : ZServices::GameObjectManager()->Components(selectedObject_))
        {
            auto type = rttr::type::get(*ZServices::ComponentManager()->Dereference(comp));
            for (auto prop : type.get_properties())
            {
                ILOG(prop.get_name().to_string());
            }
        }
    }
}