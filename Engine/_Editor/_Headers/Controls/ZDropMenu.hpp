/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
	ZDropMenu.hpp

	Created by Adrian Sanchez on 11/07/21.
	Copyright © 2021 Pervasive Sense. All rights reserved.
 
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

#include "ZEditorControl.hpp"
#include "ZUIElement.hpp"
#include "ZUIClicker.hpp"

class ZScene;
class ZFloatField;

class ZDropMenu : public ZEditorControl {

public:

    ZDropMenu(const ZUITheme& theme = ZUITheme())
        : ZEditorControl(theme)
    { }
    ~ZDropMenu() {}

    void Initialize(const std::shared_ptr<ZScene>& scene) override;

    void Update() override;

    void CleanUp() override { }

    void AddMenuItem(const std::string& label);

    static std::shared_ptr<ZDropMenu> Create(const ZHUIElement& activationElement, const ZUIElementOptions& elementOptions, const std::shared_ptr<ZScene>& scene = nullptr, ZUITheme theme = ZUITheme());

protected:

    ZHUIElement menu_;
    ZHUIElement activationElement_;
    ZUIClicker clicker_;
    std::shared_ptr<class ZScene> scene_;

};