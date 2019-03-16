//
//  ZUICheckBox.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 16/03/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

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

  void Initialize(ZOFNode* root) override;

  bool Checked() const { return checked_; }
  void SetChecked(bool checked = true) { checked_ = checked; }

  void Draw(ZShader* shader) override;
  ZMeshUI ElementShape() override;
  
protected:

  void HandleMousePress(std::shared_ptr<ZEvent> event);

};
