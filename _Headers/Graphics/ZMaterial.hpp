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
  std::string meshId_;
  int index_;
  bool isPBR_ = false;

  void SetMaterialProperty(std::string property, float value, ZMaterialProperties& materialProperties);
  void SetMaterialProperty(std::string property, glm::vec4 value, ZMaterialProperties& materialProperties);

public:

  ZMaterial(int index = 0) { index_ = index; }
  ZMaterial(ZMaterialProperties& materialProperties) : ZMaterial(0) { properties_ = materialProperties; }
  ZMaterial(std::vector<ZTexture> textures) : ZMaterial(0) { textures_ = textures; }

  // TODO: Add ZOFTree initializer. Set PBR flag based on present fields.
  // If a field is numeric then material field is purely programmatic, otherwise
  // there may be textures associated with that field.
  void Initialize(ZOFTree* root);

  static std::unique_ptr<ZMaterial> DefaultMaterialSimple();
  static std::unique_ptr<ZMaterial> DefaultMaterialPBR();

  void SetMeshID(std::string id) { meshId_ = id; }
  const std::string MeshID() const { return meshId_; }

  void SetProperties(ZMaterialProperties properties) { properties_ = properties; }
  const ZMaterialProperties& Properties() const { return properties_; }

  void AddTexture(ZTexture texture) { textures_.push_back(texture); }
  const std::vector<ZTexture>& Textures() const { return textures_; }

  int Index() const { return index_; }
  bool IsPBR() const { return isPBR_; }
  bool IsTextured() const { return !textures_.empty(); }
  void SetPBR(bool pbr = true) { isPBR_ = pbr; }

protected:

};
