/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZConsoleTool.cpp
 
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

#include "ZConsoleTool.hpp"
#include "ZEditor.hpp"

void ZConsoleTool::Begin() {
	ImGui::Begin(name_.c_str());
}

void ZConsoleTool::Update() {
    if (ImGui::BeginPopup("Options")) {
        ImGui::Checkbox("Auto-Scroll", &autoScroll_);
        ImGui::EndPopup();
    }
    
    if (ImGui::Button("Options"))
        ImGui::OpenPopup("Options");
    ImGui::SameLine();
    bool clear = ImGui::Button("Clear");
    ImGui::SameLine();
    bool copy = ImGui::Button("Copy");
    ImGui::SameLine();
    filter_.Draw("Filter", -100.f);
    
    ImGui::Separator();
    ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
    
    if (clear) zenith::Logger()->Clear();
    if (copy) ImGui::LogToClipboard();
    
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    if (filter_.IsActive()) {
        for (ZLogEntry entry : zenith::Logger()->Buffer()) {
            if (filter_.PassFilter(entry.text.c_str()))
                StyledLogEntry(entry);
        }
    } else {
        for (ZLogEntry entry : zenith::Logger()->Buffer())
            StyledLogEntry(entry);
    }
    ImGui::PopStyleVar();
    
    if (autoScroll_)
        ImGui::SetScrollHereY(1.f);
    
    ImGui::EndChild();
}

void ZConsoleTool::StyledLogEntry(ZLogEntry entry) {
    switch (entry.severity) {
        case ZINFO:
            if (editor_->Config().theme == "Light") {
                ImGui::TextColored(ImVec4(0.15, 0.15, 0.15, 1), "%s", entry.text.c_str());
            } else {
                ImGui::TextColored(ImVec4(1, 1, 1, 1), "%s", entry.text.c_str());
            }
            break;
        case ZWARNING:
            if (editor_->Config().theme == "Light") {
                ImGui::TextColored(ImVec4(0.38, 0.43, 0.04, 1), "%s", entry.text.c_str());
            } else {
                ImGui::TextColored(ImVec4(1.0, 0.85, 0.137, 1), "%s", entry.text.c_str());
            }
            break;
        case ZERROR:
            if (editor_->Config().theme == "Light") {
                ImGui::TextColored(ImVec4(0.43, 0.13, 0.13, 1), "%s", entry.text.c_str());
            } else {
                ImGui::TextColored(ImVec4(0.655, 0.161, 0.18, 1), "%s", entry.text.c_str());
            }
            break;
        default: break;
    }
}

void ZConsoleTool::End() {
	ImGui::End();
}
