/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZFoldoutRegion.hpp

    Created by Adrian Sanchez on 05/30/21.
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

#include "ZFoldoutRegion.hpp"
#include "ZUIVerticalLayout.hpp"
#include "ZUIHorizontalLayout.hpp"
#include "ZUIText.hpp"
#include "ZUIImage.hpp"
#include "ZUIPanel.hpp"
#include "ZUIHoverer.hpp"
#include "ZUIClicker.hpp"
#include "ZScene.hpp"
#include "ZDomain.hpp"
#include "ZServices.hpp"
#include "ZShader.hpp"

void ZFoldoutRegion::Initialize(const std::shared_ptr<ZScene>& scene)
{
    hoverer_ = std::make_shared<ZUIHoverer>();
    clicker_ = std::make_shared<ZUIClicker>();
    expandedMaxSize_ = container_->MaxSize();
}

void ZFoldoutRegion::Update()
{
    auto elementRect = header_->CalculatedRect();

    if (hoverer_->Entered(elementRect)) {
        header_->Scene()->Domain()->SetCursor(ZCursor(ZSystemCursorType::Hand));
    }
    else if (hoverer_->Exited(elementRect)) {
        header_->Scene()->Domain()->SetCursor(ZCursor(ZSystemCursorType::Arrow));
    }

    if (clicker_->Click(elementRect)) {
        Toggle();
    }
}

void ZFoldoutRegion::Toggle()
{
    expanded_ = !expanded_;
    arrow_->Rotate(expanded_ ? glm::radians(90.f) : glm::radians(-90.f));
    if (expanded_) {
        container_->SetMaxSize(expandedMaxSize_);
        container_->AddChild(children_);
    }
    else {
        container_->RemoveChild(children_);
        container_->SetMaxSize(glm::vec2(0.f, 30.f));
    }
}

void ZFoldoutRegion::AddChild(const std::shared_ptr<ZUIElement>& element)
{
    children_->AddChild(element);
}

void ZFoldoutRegion::RemoveChild(const std::shared_ptr<ZUIElement>& element, bool recurse)
{
    children_->RemoveChild(element, recurse);
}

std::shared_ptr<ZFoldoutRegion> ZFoldoutRegion::Create(const std::string& label, const ZUIElementOptions& options, const std::shared_ptr<ZScene>& scene, ZUITheme theme)
{
    auto foldout = std::make_shared<ZFoldoutRegion>(theme);

    ZUIElementOptions containerOptions = options;
    containerOptions.maxSize += glm::vec2(0.f, 35.f);
    containerOptions.color = theme.secondaryColor * 0.5f;
    ZUILayoutOptions layoutOptions;
    layoutOptions.itemSpacing = 5.f;
    layoutOptions.horizontalAlign = ZAlignment::Right;
    containerOptions.layout = std::make_shared<ZUIVerticalLayout>(layoutOptions);

    foldout->container_ = ZUIPanel::Create(containerOptions, scene);

    ZUIElementOptions headerOptions;
    headerOptions.positioning = ZPositioning::Relative;
    headerOptions.scaling = ZPositioning::Relative;
    headerOptions.rect = ZRect(0.f, 0.f, 1.f, 1.f);
    headerOptions.maxSize = glm::vec2(0.f, 30.f);
    headerOptions.color = theme.secondaryColor * 0.25f;
    headerOptions.color.a = 1.f;
    layoutOptions = ZUILayoutOptions();
    layoutOptions.itemSpacing = 15.f;
    layoutOptions.defaultItemSize = glm::vec2(30.f, 30.f);
    headerOptions.layout = std::make_shared<ZUIHorizontalLayout>(layoutOptions);

    foldout->header_ = ZUIPanel::Create(headerOptions, scene);
    foldout->container_->AddChild(foldout->header_);

    ZUIElementOptions imageOptions;
    imageOptions.positioning = ZPositioning::Relative;
    imageOptions.rect = ZRect(0.f, 0.f, 30.f, 30.f);
    imageOptions.color = glm::vec4(1.f);
    imageOptions.shader = ZServices::ShaderManager()->Create("/Shaders/Vertex/ui.vert", "/Shaders/Pixel/triangle.frag");

    foldout->arrow_ = ZUIImage::Create(imageOptions, scene);
    foldout->header_->AddChild(foldout->arrow_);

    ZUIElementOptions textOptions;
    textOptions.positioning = ZPositioning::Relative;
    textOptions.scaling = ZPositioning::Relative;
    textOptions.rect = ZRect(0.f, 0.f, 1.f, 1.f);
    textOptions.color = glm::vec4(1.f);

    auto transformHeaderText = ZUIText::Create(textOptions, scene);
    transformHeaderText->SetText("Transform");
    transformHeaderText->SetVerticalAlignment(ZAlignment::Middle);
    transformHeaderText->SetFontScale(16.0);
    foldout->header_->AddChild(transformHeaderText);

    ZUIElementOptions childrenOptions = options;
    childrenOptions.positioning = ZPositioning::Relative;
    childrenOptions.scaling = ZPositioning::Relative;
    childrenOptions.rect = ZRect(0.f, 0.f, 0.95f, 1.f);
    layoutOptions = ZUILayoutOptions();
    layoutOptions.itemSpacing = 5.f;
    childrenOptions.layout = std::make_shared<ZUIVerticalLayout>(layoutOptions);

    foldout->children_ = ZUIPanel::Create(childrenOptions, scene);
    foldout->container_->AddChild(foldout->children_);

    foldout->Initialize(scene);

    return foldout;
}
