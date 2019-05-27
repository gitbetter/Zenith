/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZEditor.hpp
 
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

#pragma once

// Includes
#include "ZProcess.hpp"

// Forward Declarations
class ZEditorTool;
class ZMenuBar;
class ZEvent;
class ZResourceHandle;
struct ImFont;

// Definitions
const std::string EDITOR_CONFIG_PATH = "Editor/_Assets/conf.zof";
const std::string FA_PATH = "../Editor/_Assets/Fonts/FontAwesome5_Regular_400.otf";
const std::string FA_SOLID_PATH = "../Editor/_Assets/Fonts/FontAwesome5_Solid_900.otf";

struct ZEditorConfig {
	std::string mainFontPath;
	float mainFontSize;
};

class ZEditor : public ZProcess {

private:

	ZEditorConfig config_;

    bool editorOpen_;
	ImFont* editorFont_ = nullptr;
	std::shared_ptr<ZMenuBar> menuBar_;
	std::vector<std::shared_ptr<ZEditorTool>> tools_;
	
	void SetupInitialTools();
	void SetupFontIcons();
	void ConfigSetup(std::shared_ptr<ZOFTree> objectTree);
	void ConfigSetup(ZEditorConfig config);
	void HandleResourceLoaded(std::shared_ptr<ZEvent> event);
    
public:
    
    ZEditor() : ZProcess(), editorOpen_(true) { }
    
    void Initialize() override;
    void Update() override;
	void Abort() override;
    void CleanUp() override { };

	void SetEditorFont(std::string fontPath);
	void SetEditorFontFromMemory(std::shared_ptr<ZResourceHandle> handle);
    
protected:

    void BeginFrame();
	void DockspaceBegin();
	void DockspaceEnd();
	void EndFrame();

};
