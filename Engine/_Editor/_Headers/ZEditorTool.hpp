/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZEditorTool.cpp
 
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

#pragma once

// Includes
#include "ZEngine.hpp"
#include "ZUI.hpp"

// Forward Declarations
class ZEditorScene;
class ZUIPanel;

// Definitions
class ZEditorTool {

public:

	ZEditorTool(const std::string& name, const ZUITheme& theme = ZUITheme())
		: name_(name), theme_(theme), visible_(true) { }
	virtual ~ZEditorTool() { }
    
	virtual void Initialize();

	virtual void Update() = 0;

	std::shared_ptr<ZUIPanel> Container() const { return container_; }
	
protected:

	std::shared_ptr<ZUIPanel> container_;
	std::string name_;
	ZUITheme theme_;
	bool visible_;

};
