//
//  ZGraphicsComponent.hpp
//  Starter
//
//  Created by Adrian Sanchez on 26/01/2019.
//

#pragma once

// Includes
#include "ZComponent.hpp"
#include <vector>

// Forward Declarations
class ZModel;
class ZShader;

// Class and Data Structure Definitions
class ZGraphicsComponent : ZComponent {
private:
  unsigned int currentShaderIndex_ = 0;

public:
  ZGraphicsComponent(ZModel* model, ZShader* shader);
  ~ZGraphicsComponent() { }

  virtual void Update(float frameMix);

protected:
  ZModel* model_ = nullptr;
  std::vector<ZShader*> shaders_;
};
