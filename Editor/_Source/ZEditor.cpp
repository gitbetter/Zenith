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

#include "IconsFontAwesome5.h"

#include "ZEditor.hpp"
#include "ZGame.hpp"
#include "ZDomain.hpp"
#include "ZInput.hpp"
#include "ZGraphics.hpp"
#include "ZGraphicsStrategy.hpp"
#include "ZEventAgent.hpp"
#include "ZResourceCache.hpp"
#include "ZResourceHandle.hpp"
#include "ZResourceLoadedEvent.hpp"
#include "ZResourceExtraData.hpp"

#include "ZMenuBar.hpp"
#include "ZActionBar.hpp"
#include "ZSceneTool.hpp"
#include "ZProjectTool.hpp"
#include "ZConsoleTool.hpp"
#include "ZInspectorTool.hpp"
#include "ZHierarchyTool.hpp"

#include "ZDevResourceFile.hpp"

void ZEditor::Initialize() {
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	io.Fonts->AddFontDefault();

	ImGui::StyleColorsDark();
	
	ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)zenith::Domain()->Strategy()->Context(), true);
	ImGui_ImplOpenGL3_Init("#version 400");

	SetupInitialTools();

	zenith::ResourceCache()->RegisterResourceFile(std::shared_ptr<ZDevResourceFile>(new ZDevResourceFile("../Editor/_Assets")));

	zenith::LoadZOF(EDITOR_CONFIG_PATH);

	ZEventDelegate resourceLoadedDelegate = fastdelegate::MakeDelegate(this, &ZEditor::HandleResourceLoaded);
	zenith::EventAgent()->AddListener(resourceLoadedDelegate, ZResourceLoadedEvent::Type);

	ZProcess::Initialize();
}

void ZEditor::ConfigSetup(std::shared_ptr<ZOFTree> objectTree) {
	ZEditorConfig config;
	if (objectTree->children.find("CONFIG") != objectTree->children.end()) {
		std::shared_ptr<ZOFObjectNode> configDataNode = std::dynamic_pointer_cast<ZOFObjectNode>(objectTree->children["CONFIG"]);
		if (configDataNode->properties.find("font") != configDataNode->properties.end()) {
			config.mainFontPath = configDataNode->properties["font"]->Value<ZOFString>(0)->value;
			config.mainFontSize = configDataNode->properties["font"]->Value<ZOFNumber>(1)->value;
		}
	}
	ConfigSetup(config);
}

void ZEditor::ConfigSetup(ZEditorConfig config) {
	config_ = config;

	if (!config_.mainFontPath.empty()) {
		ZResource fontResource(config_.mainFontPath, ZResourceType::Font);
		zenith::ResourceCache()->RequestHandle(fontResource);
	}
}

void ZEditor::SetEditorFontFromMemory(std::shared_ptr<ZResourceHandle> handle) {
	ImGuiIO& io = ImGui::GetIO();
	ImFontConfig fontConfig;
	fontConfig.FontDataOwnedByAtlas = false;
	editorFont_ = io.Fonts->AddFontFromMemoryTTF((void*)handle->Buffer(), handle->Size(), config_.mainFontSize, &fontConfig);

	SetupFontIcons();

	io.Fonts->Build();
	ImGui_ImplOpenGL3_CreateFontsTexture();
}

void ZEditor::SetEditorFont(std::string fontPath) {
	ImGuiIO& io = ImGui::GetIO();
	editorFont_ = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), config_.mainFontSize);

	SetupFontIcons();

	io.Fonts->Build();
	ImGui_ImplOpenGL3_CreateFontsTexture();
}

void ZEditor::SetupFontIcons() {
	ImGuiIO& io = ImGui::GetIO();
	ImFontConfig fontConfig;
	fontConfig.MergeMode = true;
	fontConfig.GlyphMinAdvanceX = 13.0f;
	static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	io.Fonts->AddFontFromFileTTF(FA_PATH.c_str(), config_.mainFontSize, &fontConfig, icon_ranges);
	io.Fonts->AddFontFromFileTTF(FA_SOLID_PATH.c_str(), config_.mainFontSize, &fontConfig, icon_ranges);
}

void ZEditor::SetupInitialTools() {
	menuBar_ = std::make_shared<ZMenuBar>();

	std::shared_ptr<ZActionBar> actionBar = std::make_shared<ZActionBar>();
	tools_.push_back(actionBar);
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

	menuBar_->Begin();
	menuBar_->Update();
	menuBar_->End();

	DockspaceBegin();
	for (std::shared_ptr<ZEditorTool> tool : tools_) {
		tool->Begin();
		tool->Update();
		tool->End();
	}
	DockspaceEnd();

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

	if (editorFont_) ImGui::PushFont(editorFont_);
}

void ZEditor::EndFrame() {
	if (editorFont_) ImGui::PopFont();

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
		ImGuiID dock_up_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.05f, nullptr, &dock_main_id);
		ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.2f, nullptr, &dock_main_id);
		ImGuiID dock_left_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.2f, nullptr, &dock_main_id);
		ImGuiID dock_down_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.2f, nullptr, &dock_main_id);
		ImGuiID dock_down_right_id = ImGui::DockBuilderSplitNode(dock_down_id, ImGuiDir_Right, 0.6f, nullptr, &dock_down_id);

		ImGui::DockBuilderDockWindow("Actions", dock_up_id);
		ImGui::DockBuilderDockWindow("Hierarchy", dock_right_id);
		ImGui::DockBuilderDockWindow("Inspector", dock_left_id);
		ImGui::DockBuilderDockWindow("Console", dock_down_id);
		ImGui::DockBuilderDockWindow("Project", dock_down_right_id);
		ImGui::DockBuilderDockWindow("Scene", dock_main_id);

		ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_up_id);
		node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoResize | ImGuiDockNodeFlags_AutoHideTabBar;

		ImGui::DockBuilderFinish(dock_main_id);
	}
	ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);
}

void ZEditor::DockspaceEnd() {
	ImGui::End();
}

void ZEditor::HandleResourceLoaded(std::shared_ptr<ZEvent> event) {
	std::shared_ptr<ZResourceLoadedEvent> loadedEvent = std::dynamic_pointer_cast<ZResourceLoadedEvent>(event);
	ZResource& resource = loadedEvent->Handle()->Resource();

	if (resource.type == ZResourceType::ZOF && resource.name == EDITOR_CONFIG_PATH) {
		std::shared_ptr<ZZOFResourceExtraData> zofData = std::dynamic_pointer_cast<ZZOFResourceExtraData>(loadedEvent->Handle()->ExtraData());
		ConfigSetup(zofData->ObjectTree());
	} else if (resource.type == ZResourceType::Font && resource.name == config_.mainFontPath) {
		SetEditorFontFromMemory(loadedEvent->Handle());
	}
}