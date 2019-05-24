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

// Definitions
class ZEditor : public ZProcess {

private:

    bool editorOpen_;
	std::shared_ptr<ZMenuBar> menuBar_;
	std::vector<std::shared_ptr<ZEditorTool>> tools_;
	
	void SetupInitialTools();
    
public:
    
    ZEditor() : ZProcess(), editorOpen_(true) { }
    
    void Initialize() override;
    void Update() override;
	void Abort() override;
    void CleanUp() override { };
    
protected:

    void BeginFrame();
	void DockspaceBegin();
	void DockspaceEnd();
	void EndFrame();

};
