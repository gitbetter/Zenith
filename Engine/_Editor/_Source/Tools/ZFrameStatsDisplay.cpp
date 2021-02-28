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
#include "ZUIPanel.hpp"
#include "ZServices.hpp"
#include "ZScene.hpp"
#include "ZUIVerticalLayout.hpp"
#include "ZUILabeledElement.hpp"
#include "ZUIText.hpp"

void ZFrameStatsDisplay::Initialize(const std::shared_ptr<ZScene>& scene)
{
    ZEditorTool::Initialize(scene);
    container_->SetColor(theme_.secondaryColor * 0.75f);
    container_->SetRect(ZRect(0.01f, 0.01f, 0.35f, 0.25f));

    ZUILayoutOptions layoutOptions;
    layoutOptions.itemSpacing = 5.f;
    layoutOptions.dimensions = container_->CalculatedRect();
    layoutOptions.padding = glm::vec2(5.f);
    container_->SetLayout(std::make_shared<ZUIVerticalLayout>(layoutOptions));

    container_->Hide();

    SetupStatFields(scene);
}

void ZFrameStatsDisplay::Update()
{
    if (ZServices::Input()->Key(ZKey::Q)) {
        if (container_->Hidden())
            container_->Show();
    }
    else {
        if (!container_->Hidden())
            container_->Hide();
    }

    if (!container_->Hidden()) {
        ZFrameStats frameStats = ZPR_SESSION_STATS(activeProjectScene_->GameName());
        drawCallsText_->SetText(std::to_string(frameStats.totalDrawCalls));
        verticesText_->SetText(std::to_string(frameStats.totalVertices));
        renderPassesText_->SetText(std::to_string(frameStats.totalRenderPasses));
        frameTimeText_->SetText(std::to_string(frameStats.frameTime));
        fpsText_->SetText(std::to_string(frameStats.framesPerSecond));
    }
}

void ZFrameStatsDisplay::SetupStatFields(const std::shared_ptr<ZScene>& scene)
{
    ZUIElementOptions textOptions;
    textOptions.positioning = ZPositioning::Relative;
    textOptions.scaling = ZPositioning::Relative;
    textOptions.rect = ZRect(0.f, 0.f, 1.f, 1.f);
    textOptions.maxSize = glm::vec2(0.f, 40.f);
    textOptions.color = glm::vec4(1.f);

    drawCallsText_ = ZUIText::Create(textOptions, scene);
    drawCallsText_->SetFontScale(14.f);
    auto drawCallsField = ZUILabeledElement::Create("Draw Calls: ", drawCallsText_);

    verticesText_ = ZUIText::Create(textOptions, scene);
    verticesText_->SetFontScale(14.f);
    auto verticesField = ZUILabeledElement::Create("Vertices: ", verticesText_);

    renderPassesText_ = ZUIText::Create(textOptions, scene);
    renderPassesText_->SetFontScale(14.f);
    auto renderPassesField = ZUILabeledElement::Create("Render Passes: ", renderPassesText_);

    frameTimeText_ = ZUIText::Create(textOptions, scene);
    frameTimeText_->SetFontScale(14.f);
    auto frameTimeField = ZUILabeledElement::Create("Frame Time: ", frameTimeText_);

    fpsText_ = ZUIText::Create(textOptions, scene);
    fpsText_->SetFontScale(14.f);
    auto fpsField = ZUILabeledElement::Create("FPS: ", fpsText_);

    container_->AddChild(drawCallsField);
    container_->AddChild(verticesField);
    container_->AddChild(renderPassesField);
    container_->AddChild(frameTimeField);
    container_->AddChild(fpsField);
}
