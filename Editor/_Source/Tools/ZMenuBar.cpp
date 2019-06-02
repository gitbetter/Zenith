/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZMenuBar.cpp
 
    Created by Adrian Sanchez on 23/05/19.
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

#include "ZMenuBar.hpp"

void ZMenuBar::Begin() {
	ImGui::BeginMainMenuBar();
}

void ZMenuBar::Update() {
	if (ImGui::BeginMenu("File")) {
		if (ImGui::BeginMenu("New")) {
			ImGui::MenuItem("Project");
			ImGui::MenuItem("Scene");
			ImGui::EndMenu();
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Scene")) {
		if (ImGui::BeginMenu("New")) {
			if (ImGui::BeginMenu("Game Object")) {
				ImGui::MenuItem("Empty");
				ImGui::MenuItem("Cube");
				ImGui::MenuItem("Sphere");
				ImGui::MenuItem("Plane");
				ImGui::EndMenu();
			}
			ImGui::MenuItem("Camera");
			ImGui::MenuItem("Light");
			ImGui::MenuItem("Trigger Volume");
			ImGui::EndMenu();
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Edit")) {
		ImGui::EndMenu();
	}
}

void ZMenuBar::End() {
	ImGui::EndMainMenuBar();
}