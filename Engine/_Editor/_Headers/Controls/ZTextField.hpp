/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZTextField.hpp
 
    Created by Adrian Sanchez on 02/15/21.
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
#include "ZEditorControl.hpp"
#include "ZUILabeledElement.hpp"

 // Forward Declarations
class ZUIScene;
class ZUIHoverer;
class ZUIInputField;

// Definitions
class ZTextField : public ZTypedEditorControl<ZUILabeledElement, std::string> {

public:

    ZTextField(const ZUITheme& theme = ZUITheme())
        : ZTypedEditorControl(theme)
    { 
        value_ = "";
    }
    ~ZTextField() {}

    void Initialize(const std::shared_ptr<ZScene>& scene) override;

    void Update() override;

    void SetValue(const std::string& val) override;

    static std::shared_ptr<ZTextField> Create(const std::string& label = "", const ZUIElementOptions& options = ZUIElementOptions(), const std::shared_ptr<ZScene>& scene = nullptr, ZUITheme theme = ZUITheme());

protected:

    std::shared_ptr<ZUIInputField> inputField_ = nullptr;
    std::shared_ptr<ZUIHoverer> hoverer_ = nullptr;

};