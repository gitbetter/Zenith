//
//  ZMesh.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 06/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZCommon.hpp"
#include "ZObject.hpp"

// Forward Declarations
class ZShader;

// Class and Data Structure Definitions
class ZMesh : public ZObject {
private:

public:

  virtual ~ZMesh() { }

  virtual void Render(ZShader* shader) = 0;
  virtual ZMeshDrawStyle DrawStyle() const { return drawStyle_; };

protected:

  ZBufferData bufferData_;
  ZMeshDrawStyle drawStyle_;

};
