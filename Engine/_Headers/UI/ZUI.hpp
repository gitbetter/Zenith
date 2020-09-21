/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZUI.hpp

    Created by Adrian Sanchez on 06/02/2019.
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
// TODO: Conditional include based on graphics implementation
#include "ZGLGraphicsStrategy.hpp"
#include "ZUIElement.hpp"
#include "ZTextStrategy.hpp"
#include <vector>

// Forward Declarations

// Class and Data Structure Definitions
class ZUI
{

public:

    ~ZUI() {}

    void Initialize();
    void RegisterFont(const std::string& fontPath);
    void Render(ZUIElementMap& elements);
    void RenderElement(const std::shared_ptr<ZUIElement>& element);
    void CleanUp();

    ZTextStrategy* TextStrategy() { return textStrategy_.get(); }
    std::shared_ptr<ZShader> TextShader() { return textShader_; }
    std::shared_ptr<ZShader> UIShader() { return uiShader_; }

protected:

    void RenderGeneric(const std::shared_ptr<ZUIElement>& element);
    void RenderImage(const std::shared_ptr<ZUIElement>& element);
    void RenderText(const std::shared_ptr<ZUIElement>& element);

protected:

    std::unique_ptr<ZTextStrategy> textStrategy_;
    std::shared_ptr<ZShader> uiShader_;
    std::shared_ptr<ZShader> textShader_;
    glm::mat4 projection_;

};
