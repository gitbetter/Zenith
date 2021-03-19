/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZVec3Field.hpp
 
    Created by Adrian Sanchez on 02/17/21.
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
class ZUIScrubber;
class ZFloatField;

// Definitions
class ZVec3Field : public ZTypedEditorControl<ZUILabeledElement, glm::vec3> {

public:

    ZVec3Field(const ZUITheme& theme = ZUITheme())
        : ZTypedEditorControl(theme)
    { }
    ~ZVec3Field() {}

    void Initialize(const std::shared_ptr<ZScene>& scene) override { }

    void Update() override;

    void SetValue(const glm::vec3& val) override;

    static std::shared_ptr<ZVec3Field> Create(const std::string& label = "", const ZUIElementOptions& options = ZUIElementOptions(), const std::shared_ptr<ZScene>& scene = nullptr, ZUITheme theme = ZUITheme());

protected:

    std::shared_ptr<ZFloatField> xInputField_ = nullptr;
    std::shared_ptr<ZFloatField> yInputField_ = nullptr;
    std::shared_ptr<ZFloatField> zInputField_ = nullptr;

};