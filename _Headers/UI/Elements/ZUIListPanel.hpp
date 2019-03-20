//
//  ZUIListPanel.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 17/03/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

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
