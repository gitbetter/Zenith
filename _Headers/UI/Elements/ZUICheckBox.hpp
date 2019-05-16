/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZUICheckBox.hpp

    Created by Adrian Sanchez on 16/03/2019.
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
class ZUIImage;

// Class and Data Structure Definitions
class ZUICheckBox : public ZUIElement {

private:

  std::shared_ptr<ZUIImage> checkImage_;
  bool checked_ = false;

public:

  ZUICheckBox(glm::vec2 position = glm::vec2(0.1f), glm::vec2 scale = glm::vec2(0.07f, 0.03f));
  ~ZUICheckBox() { }

  void Initialize(std::shared_ptr<ZOFNode> root) override;

  bool Checked() const { return checked_; }
  void SetChecked(bool checked = true) { checked_ = checked; }

  void Render(float frameMix = 1.f, ZRenderOp renderOp = ZRenderOp::Color) override;
  
protected:

  void HandleMousePress(std::shared_ptr<ZEvent> event);

};
