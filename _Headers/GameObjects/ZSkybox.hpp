//
//  ZSkybox.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 02/03/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZCommon.hpp"
#include "ZGameObject.hpp"

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
class ZSkybox: public ZGameObject {

public:

  ZSkybox() : ZGameObject(glm::vec3(0.f)) { }
  ~ZSkybox() { }

  void Initialize(std::string hdrMap);

  ZIBLTexture IBLTexture() const { return iblTexture_; }

protected:

  ZIBLTexture iblTexture_;

};
