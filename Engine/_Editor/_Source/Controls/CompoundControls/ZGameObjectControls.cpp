/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZGameObjectControls.hpp

    Created by Adrian Sanchez on 03/13/21.
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

#include "ZGameObjectControls.hpp"
#include "ZUIPanel.hpp"
#include "ZUIImage.hpp"
#include "ZUIHorizontalLayout.hpp"
#include "ZUIVerticalLayout.hpp"
#include "ZTextField.hpp"
#include "ZBoolField.hpp"
#include "ZVec3Field.hpp"
#include "ZFoldoutRegion.hpp"
#include "ZGameObject.hpp"

void ZGameObjectControls::Initialize(const std::shared_ptr<ZScene>& scene)
{
    SetupObjectHeader(scene);
    SetupTransformFields(scene);
}

void ZGameObjectControls::Update()
{
    if (!gameObject_) return;

    auto name = gameObject_->ID();
    auto active = gameObject_->Active();

    auto m = gameObject_->ModelMatrix();
    glm::vec3 pos, scale, orn;
    glm::quat rot;
    glm::decompose(m, scale, rot, pos, glm::vec3(0.f), glm::vec4(0.f));
    orn = glm::eulerAngles(rot);

    nameField_->SetValue(name);
    activeField_->SetValue(active);
    positionField_->SetValue(pos);
    scaleField_->SetValue(scale);
    rotationField_->SetValue(orn);

    nameField_->Update();
    activeField_->Update();
    transformFields_->Update();
    positionField_->Update();
    scaleField_->Update();
    rotationField_->Update();

    if (activeField_->Value(active)) {
        gameObject_->SetActive(active);
    }
    if (positionField_->Value(pos)) {
        gameObject_->SetPosition(pos);
    }
    if (scaleField_->Value(scale)) {
        gameObject_->SetScale(scale);
    }
    if (rotationField_->Value(orn)) {
        gameObject_->SetOrientation(orn);
    }
}

void ZGameObjectControls::SetupObjectHeader(const std::shared_ptr<ZScene>& scene)
{
    ZUIElementOptions fieldOptions;
    fieldOptions.positioning = ZPositioning::Relative;
    fieldOptions.scaling = ZPositioning::Relative;
    fieldOptions.rect = ZRect(0.0f, 0.0f, 1.0f, 1.0f);
    fieldOptions.maxSize = glm::vec2(0.f, 75.f);
    ZUILayoutOptions layoutOptions;
    layoutOptions.itemSpacing = 5.f;
    fieldOptions.layout = std::make_shared<ZUIHorizontalLayout>(layoutOptions);
    objectHeader_ = ZUIPanel::Create(fieldOptions, scene);

    fieldOptions = ZUIElementOptions();
    fieldOptions.positioning = ZPositioning::Relative;
    fieldOptions.rect.size = glm::vec2(50.f, 50.f);
    fieldOptions.color = glm::vec4(1.f, 1.f, 1.f, 1.f);
    fieldOptions.flipped = true;
    auto objectIcon = ZUIImage::Create(fieldOptions, scene);
    objectIcon->SetImage("/Images/object_cube.png");

    objectHeader_->AddChild(objectIcon);

    SetupObjectHeaderFields(scene);
}

void ZGameObjectControls::SetupObjectHeaderFields(const std::shared_ptr<ZScene>& scene)
{
    ZUIElementOptions fieldOptions;
    fieldOptions.positioning = ZPositioning::Relative;
    fieldOptions.scaling = ZPositioning::Relative;
    fieldOptions.rect = ZRect(0.0f, 0.0f, 0.85f, 0.85f);
    fieldOptions.padding = glm::vec2(10.f, 5.f);
    ZUILayoutOptions layoutOptions;
    layoutOptions.itemSpacing = 5.f;
    fieldOptions.layout = std::make_shared<ZUIVerticalLayout>(layoutOptions);
    auto headerFieldsContainer = ZUIPanel::Create(fieldOptions, scene);

    objectHeader_->AddChild(headerFieldsContainer);

    fieldOptions = ZUIElementOptions();
    fieldOptions.positioning = ZPositioning::Relative;
    fieldOptions.scaling = ZPositioning::Relative;
    fieldOptions.rect = ZRect(0.0f, 0.0f, 1.f, 1.f);
    fieldOptions.maxSize = glm::vec2(0.f, 20.f);
    layoutOptions = ZUILayoutOptions();
    layoutOptions.itemSpacing = 10.f;
    fieldOptions.layout = std::make_shared<ZUIHorizontalLayout>(layoutOptions);
    auto nameFieldContainer = ZUIPanel::Create(fieldOptions, scene);

    headerFieldsContainer->AddChild(nameFieldContainer);

    fieldOptions = ZUIElementOptions();
    fieldOptions.positioning = ZPositioning::Relative;
    fieldOptions.scaling = ZPositioning::Relative;
    fieldOptions.rect = ZRect(0.0f, 0.0f, 0.90f, 1.f);
    fieldOptions.color = theme_.primaryColor;
    nameField_ = ZTextField::Create("", fieldOptions, scene, theme_);
    nameFieldContainer->AddChild(nameField_->Control());

    fieldOptions = ZUIElementOptions();
    fieldOptions.positioning = ZPositioning::Relative;
    fieldOptions.rect = ZRect(0.0f, 0.0f, 20.f, 20.f);
    fieldOptions.color = theme_.primaryColor;
    activeField_ = ZBoolField::Create("", fieldOptions, scene, theme_);
    nameFieldContainer->AddChild(activeField_->Control());
}

void ZGameObjectControls::SetupTransformFields(const std::shared_ptr<ZScene>& scene)
{
    ZUIElementOptions foldoutOptions;
    foldoutOptions.positioning = ZPositioning::Relative;
    foldoutOptions.scaling = ZPositioning::Relative;
    foldoutOptions.rect = ZRect(0.0f, 0.0f, 1.0f, 1.0f);
    foldoutOptions.maxSize = glm::vec2(0.f, 3 * (20.f + 5.f));
    transformFields_ = ZFoldoutRegion::Create("Transform", foldoutOptions, scene, theme_);

    ZUIElementOptions fieldOptions;
    fieldOptions.positioning = ZPositioning::Relative;
    fieldOptions.scaling = ZPositioning::Relative;
    fieldOptions.color = theme_.primaryColor;
    fieldOptions.rect = ZRect(0.0f, 0.0f, 1.0f, 1.0f);
    fieldOptions.maxSize = glm::vec2(0.f, 20.f);

    positionField_ = ZVec3Field::Create("Position", fieldOptions, scene, theme_);
    transformFields_->AddChild(positionField_->Control());
    scaleField_ = ZVec3Field::Create("Scale", fieldOptions, scene, theme_);
    transformFields_->AddChild(scaleField_->Control());
    rotationField_ = ZVec3Field::Create("Rotation", fieldOptions, scene, theme_);
    transformFields_->AddChild(rotationField_->Control());
}

std::shared_ptr<ZGameObjectControls> ZGameObjectControls::Create(const std::shared_ptr<ZGameObject>& object, const std::shared_ptr<ZScene>& scene, ZUITheme theme)
{
    auto controls = std::make_shared<ZGameObjectControls>(object, theme);
    controls->Initialize(scene);
    return controls;
}