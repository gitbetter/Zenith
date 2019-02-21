//
//  ZMaterial.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 01/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZCommon.hpp"

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
class ZMaterial {
private:

  ZMaterialProperties properties_;
  std::vector<ZTexture> textures_;
  int index_;

public:

  ZMaterial(int index = 0) { index_ = index; }
  ZMaterial(ZMaterialProperties& materialProperties) : ZMaterial(0) { properties_ = materialProperties; }
  ZMaterial(std::vector<ZTexture> textures) : ZMaterial(0) { textures_ = textures; }

  static ZMaterial DefaultMaterial();

  void SetProperties(ZMaterialProperties properties) { properties_ = properties; }
  const ZMaterialProperties& Properties() const { return properties_; }

  void AddTexture(ZTexture texture) { textures_.push_back(texture); }
  const std::vector<ZTexture>& Textures() const { return textures_; }

  int Index() const { return index_; }
protected:


};
