/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZUICursor.hpp

    Created by Adrian Sanchez on 09/02/2019.
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
#include "ZUIElement.hpp"

// Forward Declarations
class ZShader;

// Class and Data Structure Definitions
class ZUICursor : public ZUIElement {

private:

  float cursorSpeed_ = 10.f;

  void HandleMouseMove(std::shared_ptr<ZEvent> event);
  void HandleMousePress(std::shared_ptr<ZEvent> event);

public:

  ZUICursor(glm::vec2 position = glm::vec2(0.f), glm::vec2 scale = glm::vec2(25.f)) : ZUIElement(position, scale) { }
  ~ZUICursor() { }

  void Initialize(std::shared_ptr<ZOFNode> root) override;

  void Render(float frameMix = 1.f, ZRenderOp renderOp = ZRenderOp::Color) override;
  std::shared_ptr<ZMesh2D> ElementShape() override;

  void SetCursorImage(std::string path);
  void SetColor(glm::vec4 color) override;
  void SetCursorSpeed(float sensitivity) { cursorSpeed_ = sensitivity; }

};
