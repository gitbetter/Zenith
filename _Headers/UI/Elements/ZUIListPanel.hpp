/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZUIListPanel.hpp

    Created by Adrian Sanchez on 17/03/2019.
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
class ZUIListPanel : public ZUIElement {

private:

  float itemHeight_ = 25.f;

public:

  ZUIListPanel(glm::vec2 position = glm::vec2(0.f), glm::vec2 scale = glm::vec2(1.f)) : ZUIElement(position, scale) { }
  ~ZUIListPanel() { }

  void Initialize(ZOFNode* root) override;

  float ItemHeight() const { return itemHeight_; }
  void SetItemHeight(float itemHeight) { itemHeight_ = itemHeight; }

  void Draw(ZShader* shader) override;
  ZMeshUI ElementShape() override;

  void AddChild(std::shared_ptr<ZUIElement> element) override;

protected:

};
