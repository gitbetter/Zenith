//
//  ZUIImage.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 06/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZUIElement.hpp"
#include "ZCommon.hpp"

// Forward Declarations
class ZShader;

// Class and Data Structure Definitions
class ZUIImage : public ZUIElement {
private:

public:

  ZUIImage(std::string path = "", glm::vec2 position = glm::vec2(0.5f), glm::vec2 scale = glm::vec2(0.25f));
  ~ZUIImage() { }

  void Draw(ZShader* shader) override;
  ZMeshUI ElementShape() override;

  void SetImage(std::string path);

protected:

};
