/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZActionBar.cpp
 
    Created by Adrian Sanchez on 26/05/19.
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

#include "ZActionBar.hpp"
#include "ZServices.hpp"
#include "ZUIPanel.hpp"
#include "ZUIButton.hpp"
#include "ZUIImage.hpp"
#include "ZCamera.hpp"
#include "ZEditorScene.hpp"
#include "ZUIHorizontalLayout.hpp"
#include "ZAnimatorComponent.hpp"

void ZActionBar::Initialize(const std::shared_ptr<ZScene>& scene) {
    ZEditorTool::Initialize(scene);
    container_->SetColor(theme_.primaryColor);
    container_->SetRect(ZRect(0.f, 0.35f, 1.f, 0.65f));

    ZUIElementOptions listOptions;
    listOptions.positioning = ZPositioning::Relative;
    listOptions.scaling = ZPositioning::Relative;
    listOptions.rect = ZRect(0.005f, 0.f, 0.2f, 1.0f);
    ZUILayoutOptions layoutOptions;
    layoutOptions.itemSpacing = 10.f;
    layoutOptions.verticalAlign = ZAlignment::Middle;
    listOptions.layout = std::make_shared<ZUIHorizontalLayout>(layoutOptions);
    auto actionButtonList = ZUIPanel::Create(listOptions, scene);

    playButton_ = CreateActionButton("/Images/play_icon.png", scene);
    pauseButton_ = CreateActionButton("/Images/pause_icon.png", scene);
    stopButton_ = CreateActionButton("/Images/stop_icon.png", scene);

    actionButtonList->AddChild(playButton_);
    actionButtonList->AddChild(pauseButton_);
    actionButtonList->AddChild(stopButton_);

    container_->AddChild(actionButtonList);
}

void ZActionBar::Update() {
    if (playButton_->Clicked()) {
        activeProjectScene_->Play();
        ZServices::Input()->CaptureCursor();

        if (auto wolf = activeProjectScene_->FindGameObject("ZGO_14")) {
            auto animComp = wolf->FindComponent<ZAnimatorComponent>();
            animComp->Play("Wolf_Skeleton", true);
        }
    }
    if (stopButton_->Clicked()) {
        activeProjectScene_->Stop();
    }
    if (pauseButton_->Clicked()) {
        activeProjectScene_->Pause();
    }
}

std::shared_ptr<ZUIButton> ZActionBar::CreateActionButton(const std::string& iconPath, const std::shared_ptr<ZScene>& scene)
{
    ZUIElementOptions buttonOptions;
    buttonOptions.positioning = ZPositioning::Relative;
    buttonOptions.rect = ZRect(0.f, 0.f, 30.0f, 30.0f);
    auto button = ZUIButton::Create(buttonOptions, scene);
    ZUIElementOptions iconOptions;
    iconOptions.positioning = ZPositioning::Relative;
    iconOptions.scaling = ZPositioning::Relative;
    iconOptions.rect = ZRect(0.f, 0.f, 1.f, 1.f);
    iconOptions.color = glm::vec4(1.f, 1.f, 1.f, 1.f);
    auto icon = ZUIImage::Create(iconOptions, scene);
    icon->SetImage(iconPath);
    button->AddChild(icon);

    return button;
}
