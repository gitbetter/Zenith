/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZFloatField.hpp
 
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

#include "ZEditorControl.hpp"
#include "ZUILabeledElement.hpp"
#include "ZUIScrubber.hpp"
#include "ZUIHoverer.hpp"

class ZUIScene;

class ZFloatField : public ZTypedEditorControl<float> {

public:

    ZFloatField(const ZUITheme& theme = ZUITheme())
        : ZTypedEditorControl(theme)
    {
        value_ = 0;
    }
    ~ZFloatField() = default;

    void Initialize(const std::shared_ptr<ZScene>& scene) override;

    void Update() override;

    void SetValue(const float& val) override;

    static std::shared_ptr<ZFloatField> Create(const std::string& label = "", const ZUIElementOptions& options = ZUIElementOptions(), const std::shared_ptr<ZScene>& scene = nullptr, ZUITheme theme = ZUITheme());

protected:

    ZHUIElement inputField_;
    ZUIScrubber scrubber_;
    ZUIHoverer hoverer_;
    float lastScrubbedValue_{ 0.f };

};