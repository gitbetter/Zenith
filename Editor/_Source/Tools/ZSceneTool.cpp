/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZSceneTool.cpp
 
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

#include "ZSceneTool.hpp"
#include "ZEngine.hpp"
#include "ZGraphics.hpp"
#include "ZDomain.hpp"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void ZSceneTool::Begin() {
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin(name_.c_str(), &visible_, flags);
}

void ZSceneTool::Update() {
	ImGui::PopStyleVar();
	unsigned int width, height;
	UpdateViewportResolution(width, height);
	ImGui::Image((ImTextureID)zenith::Graphics()->SceneBuffer().id, ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0));
}

void ZSceneTool::End() {
	ImGui::End();
}

void ZSceneTool::UpdateViewportResolution(unsigned int& outWidth, unsigned int& outHeight) {
	unsigned int width = ImGui::GetWindowSize().x;
	unsigned int height = (float)width / zenith::Domain()->Aspect();

	width -= (width % 2 != 0) ? 1 : 0;
	height -= (height % 2 != 0) ? 1 : 0;

	ImGui::SetCursorPosY((ImGui::GetWindowSize().y - height) * 0.5f);

	outWidth = width; outHeight = height;
}