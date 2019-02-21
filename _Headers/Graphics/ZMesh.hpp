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

// Forward Declarations
class ZShader;

// Class and Data Structure Definitions
class ZMesh {
private:

public:

  virtual ~ZMesh() { }

  virtual void Render(ZShader* shader) = 0;

protected:

  ZBufferData bufferData_;
};
