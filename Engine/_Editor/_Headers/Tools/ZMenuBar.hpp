/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZMenuBar.hpp
 
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

#pragma once

#include "ZEditorTool.hpp"
#include "ZUIHoverer.hpp"
#include "ZUIClicker.hpp"

class ZMenuBar : public ZEditorTool
{

public:

    ZMenuBar(const ZUITheme& theme = ZUITheme())
        : ZEditorTool("MenuBar", theme) { }

    void Initialize(const std::shared_ptr<class ZScene>& scene) override;
	void Update() override;
    void CleanUp() override { }

    void AddMenuOption(const std::string& label);

protected:
    std::vector<ZHUIElement> menuItems_;
    std::vector<ZUIClicker> menuItemClickers_;
    ZHUIElement menuLayoutPanel_;
    ZUIHoverer hoverer_;

    void SetupMenuLayout(const std::shared_ptr<ZScene>& scene);

};
