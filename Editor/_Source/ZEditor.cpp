/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZEditor.cpp
 
    Created by Adrian Sanchez on 17/05/19.
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

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "ZEditor.hpp"
#include "ZEngine.hpp"
#include "ZGame.hpp"
#include "ZDomain.hpp"
#include "ZGraphics.hpp"
#include "ZGraphicsStrategy.hpp"

#include "ZSceneTool.hpp"
#include "ZProjectTool.hpp"
#include "ZConsoleTool.hpp"
#include "ZInspectorTool.hpp"
#include "ZHierarchyTool.hpp"

void ZEditor::Initialize() {
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();
	
	ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)zenith::Domain()->Strategy()->Context(), true);
	ImGui_ImplOpenGL3_Init("#version 400");

	SetupInitialTools();

	ZProcess::Initialize();
}

void ZEditor::SetupInitialTools() {
	std::shared_ptr<ZSceneTool> sceneTool = std::make_shared<ZSceneTool>();
	tools_.push_back(sceneTool);
	std::shared_ptr<ZProjectTool> projectTool = std::make_shared<ZProjectTool>();
	tools_.push_back(projectTool);
	std::shared_ptr<ZConsoleTool> consoleTool = std::make_shared<ZConsoleTool>();
	tools_.push_back(consoleTool);
	std::shared_ptr<ZInspectorTool> inspectorTool = std::make_shared<ZInspectorTool>();
	tools_.push_back(inspectorTool);
	std::shared_ptr<ZHierarchyTool> hierarchyTool = std::make_shared<ZHierarchyTool>();
	tools_.push_back(hierarchyTool);
}

void ZEditor::Update() {
	BeginFrame();
	
	for (std::shared_ptr<ZEditorTool> tool : tools_) {
		tool->Begin();
		tool->Update();
		tool->End();
	}

	EndFrame();

	ZProcess::Update();
}

void ZEditor::Abort() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	ZProcess::Abort();
}

void ZEditor::BeginFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	DockspaceBegin();
}

void ZEditor::EndFrame() {
	DockspaceEnd();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		GLFWwindow* backup_current_context = static_cast<GLFWwindow*>(zenith::Domain()->Strategy()->Context());
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		zenith::Domain()->Strategy()->SetContext(backup_current_context);
	}
}

void ZEditor::DockspaceBegin() {
	ImGuiWindowFlags windowFlags =
		ImGuiWindowFlags_MenuBar |
		ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Zenith Editor", &editorOpen_, windowFlags);

	ImGui::PopStyleVar(3);

	ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;
	ImGuiID dockspaceID = ImGui::GetID(ID().c_str());
	if (!ImGui::DockBuilderGetNode(dockspaceID)) {
		ImGui::DockBuilderRemoveNode(dockspaceID);
		ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_None);

		ImGuiID dock_main_id = dockspaceID;
		ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.2f, nullptr, &dock_main_id);
		ImGuiID dock_left_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.2f, nullptr, &dock_main_id);
		ImGuiID dock_down_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.2f, nullptr, &dock_main_id);
		const ImGuiID dock_down_right_id = ImGui::DockBuilderSplitNode(dock_down_id, ImGuiDir_Right, 0.6f, nullptr, &dock_down_id);

		ImGui::DockBuilderDockWindow("Hierarchy", dock_right_id);
		ImGui::DockBuilderDockWindow("Inspector", dock_left_id);
		ImGui::DockBuilderDockWindow("Console", dock_down_id);
		ImGui::DockBuilderDockWindow("Project", dock_down_right_id);
		ImGui::DockBuilderDockWindow("Scene", dock_main_id);
		ImGui::DockBuilderFinish(dock_main_id);
	}
	ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);
}

void ZEditor::DockspaceEnd() {
	ImGui::End();
}
