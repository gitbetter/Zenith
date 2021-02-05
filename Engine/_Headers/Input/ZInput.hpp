/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZInput.hpp

    Created by Adrian Sanchez on 28/01/2019.
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
class ZObject;

// Class and Data Structure Definitions
class ZInput : public ZProcess
{

public:

    virtual ~ZInput() {}

    virtual void Initialize() { ZProcess::Initialize(); }
    virtual glm::vec2 GetCursorPosition() = 0;
    virtual void SetCursorPosition(const glm::vec2& position) = 0;

    inline bool Key(const ZKey& key) { return keyPress_.find(key) != keyPress_.end() && keyPress_[key]; }
    inline bool Mouse(const ZMouse& mouse) { return mousePress_.find(mouse) != mousePress_.end() && mousePress_[mouse]; }

    void SetKey(const ZKey& key, bool pressed = false);
    void SetMouse(const ZMouse& mouse, bool pressed = false);

    virtual void PollEvents() = 0;
    virtual void RefreshContext() = 0;
    virtual void CaptureCursor() = 0;
    virtual void ReleaseCursor() = 0;
    virtual void HideCursor() = 0;
    virtual void ShowCursor() = 0;
    virtual bool IsCursorCaptured() = 0;
    virtual bool IsCursorHidden() = 0;

protected:

    std::map<ZKey, bool> prevKeyPress_;
    std::map<ZKey, bool> keyPress_;
    std::map<ZMouse, bool> prevMousePress_;
    std::map<ZMouse, bool> mousePress_;

};
