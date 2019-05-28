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

namespace ImGui {
    void StyleColorsLightGreen(ImGuiStyle* dst = nullptr);
}

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
		node->LocalFlags |= (ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoResize);

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

// generic miniDart theme
void ImGui::StyleColorsLightGreen(ImGuiStyle* dst)
{
    ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
    ImVec4* colors = style->Colors;
    
    style->WindowRounding    = 2.0f;             // Radius of window corners rounding. Set to 0.0f to have rectangular windows
    style->ScrollbarRounding = 3.0f;             // Radius of grab corners rounding for scrollbar
    style->GrabRounding      = 2.0f;             // Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.
    style->AntiAliasedLines  = true;
    style->AntiAliasedFill   = true;
    style->WindowRounding    = 2;
    style->ChildRounding     = 2;
    style->ScrollbarSize     = 16;
    style->ScrollbarRounding = 3;
    style->GrabRounding      = 2;
    style->ItemSpacing.x     = 10;
    style->ItemSpacing.y     = 4;
    style->IndentSpacing     = 22;
    style->FramePadding.x    = 6;
    style->FramePadding.y    = 4;
    style->Alpha             = 1.0f;
    style->FrameRounding     = 3.0f;
    
    colors[ImGuiCol_Text]                   = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]          = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_WindowBg]              = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    //colors[ImGuiCol_ChildWindowBg]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.93f, 0.93f, 0.93f, 0.98f);
    colors[ImGuiCol_Border]                = ImVec4(0.71f, 0.71f, 0.71f, 0.08f);
    colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.04f);
    colors[ImGuiCol_FrameBg]               = ImVec4(0.71f, 0.71f, 0.71f, 0.55f);
    colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.94f, 0.94f, 0.94f, 0.55f);
    colors[ImGuiCol_FrameBgActive]         = ImVec4(0.71f, 0.78f, 0.69f, 0.98f);
    colors[ImGuiCol_TitleBg]               = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.82f, 0.78f, 0.78f, 0.51f);
    colors[ImGuiCol_TitleBgActive]         = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
    colors[ImGuiCol_MenuBarBg]             = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.20f, 0.25f, 0.30f, 0.61f);
    colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.90f, 0.90f, 0.90f, 0.30f);
    colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.92f, 0.92f, 0.92f, 0.78f);
    colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_CheckMark]             = ImVec4(0.184f, 0.407f, 0.193f, 1.00f);
    colors[ImGuiCol_SliderGrab]            = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
    colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Button]                = ImVec4(0.71f, 0.78f, 0.69f, 0.40f);
    colors[ImGuiCol_ButtonHovered]         = ImVec4(0.725f, 0.805f, 0.702f, 1.00f);
    colors[ImGuiCol_ButtonActive]          = ImVec4(0.793f, 0.900f, 0.836f, 1.00f);
    colors[ImGuiCol_Header]                = ImVec4(0.71f, 0.78f, 0.69f, 0.31f);
    colors[ImGuiCol_HeaderHovered]         = ImVec4(0.71f, 0.78f, 0.69f, 0.80f);
    colors[ImGuiCol_HeaderActive]          = ImVec4(0.71f, 0.78f, 0.69f, 1.00f);
    colors[ImGuiCol_Column]                = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    colors[ImGuiCol_ColumnHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
    colors[ImGuiCol_ColumnActive]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator]              = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.14f, 0.44f, 0.80f, 0.78f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.14f, 0.44f, 0.80f, 1.00f);
    colors[ImGuiCol_ResizeGrip]            = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
    colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 0.45f);
    colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
    colors[ImGuiCol_PlotLines]             = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_ModalWindowDarkening]  = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_NavHighlight]           = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
}
