/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZEditorControl.cpp

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

#include "ZCommon.hpp"

class ZEditorControl
{
public:

    ZEditorControl(const ZUITheme& theme = ZUITheme())
        : theme_(theme) { }
    virtual ~ZEditorControl() { }

    virtual void Initialize(const std::shared_ptr<ZScene>& scene) = 0;

    virtual void Update() = 0;

    virtual void CleanUp() = 0;

protected:

    ZUITheme                                    theme_;

};

template<typename V>
class ZTypedEditorControl : public ZEditorControl {

public:

    ZTypedEditorControl(const ZUITheme& theme = ZUITheme())
        : ZEditorControl(theme) { }
    virtual ~ZTypedEditorControl() { }

    virtual void Initialize(const std::shared_ptr<ZScene>& scene) override = 0;

    virtual void Update() override = 0;

    virtual void SetValue(const V& val) = 0;

    virtual void CleanUp() override { }

    ZHUIElement Control() const { return control_; }
    bool Value(V& ref) const { ref = value_; return lastValue_ != value_; }

protected:

    ZHUIElement                          control_;
    V                                    value_, lastValue_;

};
