/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZFrameStatsDisplay.cpp

    Created by Adrian Sanchez on 02/27/19.
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

#include "ZFrameStatsDisplay.hpp"
#include "ZServices.hpp"
#include "ZAssets.hpp"
#include "ZUIPanel.hpp"
#include "ZScene.hpp"
#include "ZUIVerticalLayout.hpp"
#include "ZUILabeledElement.hpp"
#include "ZUIText.hpp"

void ZFrameStatsDisplay::Initialize(const std::shared_ptr<ZScene>& scene)
{
    ZEditorTool::Initialize(scene);

    ZAssets::UIElementManager()->SetColor(container_, theme_.secondaryColor * 0.75f);
    ZAssets::UIElementManager()->SetRect(container_, ZRect(0.01f, 0.01f, 0.35f, 0.25f));
    ZAssets::UIElementManager()->SetPadding(container_, glm::vec2(5.f));

    ZUILayoutOptions layoutOptions;
    layoutOptions.itemSpacing = 5.f;
    layoutOptions.dimensions = ZAssets::UIElementManager()->CalculatedRect(container_);
    ZAssets::UIElementManager()->SetLayout(container_, std::make_shared<ZUIVerticalLayout>(layoutOptions));

    ZAssets::UIElementManager()->Hide(container_);

    SetupStatFields(scene);
}

void ZFrameStatsDisplay::Update()
{
    if (ZServices::Input()->Key(ZKey::Q))
    {
        if (ZAssets::UIElementManager()->Hidden(container_))
        {
            ZAssets::UIElementManager()->Show(container_);
        }
    }
    else
    {
        if (!ZAssets::UIElementManager()->Hidden(container_))
        {
            ZAssets::UIElementManager()->Hide(container_);
        }
    }

    if (!ZAssets::UIElementManager()->Hidden(container_))
    {
        ZFrameStats frameStats = ZPR_SESSION_STATS(activeProjectScene_->GameName());
        ZAssets::UIElementManager()->Dereference<ZUIText>(drawCallsText_)->SetText(std::to_string(frameStats.totalDrawCalls));
        ZAssets::UIElementManager()->Dereference<ZUIText>(verticesText_)->SetText(std::to_string(frameStats.totalVertices));
        ZAssets::UIElementManager()->Dereference<ZUIText>(renderPassesText_)->SetText(std::to_string(frameStats.totalRenderPasses));
        ZAssets::UIElementManager()->Dereference<ZUIText>(frameTimeText_)->SetText(std::to_string(frameStats.frameTime));
        ZAssets::UIElementManager()->Dereference<ZUIText>(fpsText_)->SetText(std::to_string(frameStats.framesPerSecond));
    }
}

void ZFrameStatsDisplay::SetupStatFields(const std::shared_ptr<ZScene>& scene)
{
    ZUIElementOptions textOptions;
    textOptions.positioning = ZPositioning::Relative;
    textOptions.scaling = ZPositioning::Relative;
    textOptions.rect = ZRect(0.f, 0.f, 1.f, 1.f);
    textOptions.maxSize = glm::vec2(0.f, 25.f);
    textOptions.color = glm::vec4(1.f);

    drawCallsText_ = ZAssets::UIElementManager()->Create(ZUIElementType::Text, textOptions, ZHUIElement(), scene);
    ZAssets::UIElementManager()->Dereference<ZUIText>(drawCallsText_)->SetFontScale(14.f);
    auto drawCallsField = ZUILabeledElement::Create("Draw Calls: ", drawCallsText_, scene);

    verticesText_ = ZAssets::UIElementManager()->Create(ZUIElementType::Text, textOptions, ZHUIElement(), scene);
    ZAssets::UIElementManager()->Dereference<ZUIText>(verticesText_)->SetFontScale(14.f);
    auto verticesField = ZUILabeledElement::Create("Vertices: ", verticesText_, scene);

    renderPassesText_ = ZAssets::UIElementManager()->Create(ZUIElementType::Text, textOptions, ZHUIElement(), scene);
    ZAssets::UIElementManager()->Dereference<ZUIText>(renderPassesText_)->SetFontScale(14.f);
    auto renderPassesField = ZUILabeledElement::Create("Render Passes: ", renderPassesText_, scene);

    frameTimeText_ = ZAssets::UIElementManager()->Create(ZUIElementType::Text, textOptions, ZHUIElement(), scene);
    ZAssets::UIElementManager()->Dereference<ZUIText>(frameTimeText_)->SetFontScale(14.f);
    auto frameTimeField = ZUILabeledElement::Create("Frame Time: ", frameTimeText_, scene);

    fpsText_ = ZAssets::UIElementManager()->Create(ZUIElementType::Text, textOptions, ZHUIElement(), scene);
    ZAssets::UIElementManager()->Dereference<ZUIText>(fpsText_)->SetFontScale(14.f);
    auto fpsField = ZUILabeledElement::Create("FPS: ", fpsText_, scene);

    ZAssets::UIElementManager()->AddChild(container_, drawCallsField);
    ZAssets::UIElementManager()->AddChild(container_, verticesField);
    ZAssets::UIElementManager()->AddChild(container_, renderPassesField);
    ZAssets::UIElementManager()->AddChild(container_, frameTimeField);
    ZAssets::UIElementManager()->AddChild(container_, fpsField);
}
