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
#include "ZDomain.hpp"
#include "ZGame.hpp"
#include "IconsFontAwesome5.h"

void ZActionBar::Begin() {
	ImGui::Begin(name_.c_str());
}

void ZActionBar::Update() {
	ImGui::SetCursorPos(ImVec2(15.f, ImGui::GetWindowHeight() * 0.2f));
	if (ImGui::Button(ICON_FA_ARROWS_ALT, ImVec2(ImGui::GetWindowHeight() * 0.6f, ImGui::GetWindowHeight() * 0.6f))) { }
	if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Translate");
	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_SYNC, ImVec2(ImGui::GetWindowHeight() * 0.6f, ImGui::GetWindowHeight() * 0.6f))) { }
	if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Rotate");
	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_EXPAND, ImVec2(ImGui::GetWindowHeight() * 0.6f, ImGui::GetWindowHeight() * 0.6f))) { }
	if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Scale");

	ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() / 2.f, ImGui::GetWindowHeight() * 0.2f));
	if (ImGui::Button(ICON_FA_PLAY, ImVec2(ImGui::GetWindowHeight() * 0.6f, ImGui::GetWindowHeight() * 0.6f))) {
        zenith::Game()->ActiveScene()->Play();
    }
	if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Play");
	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_PAUSE, ImVec2(ImGui::GetWindowHeight() * 0.6f, ImGui::GetWindowHeight() * 0.6f))) {
        zenith::Game()->ActiveScene()->Pause();
    }
	if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Pause");
	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_STOP, ImVec2(ImGui::GetWindowHeight() * 0.6f, ImGui::GetWindowHeight() * 0.6f))) {
        zenith::Game()->ActiveScene()->Stop();
    }
	if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Stop");
}

void ZActionBar::End() {
	ImGui::End();
}
