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
class ZCamera;

// Class and Data Structure Definitions
class ZGraphicsComponent : ZComponent {
private:
  unsigned int activeShaderIndex_ = -1;

public:
  ZGraphicsComponent(ZModel* model, ZShader* shader);
  ~ZGraphicsComponent() { }

  virtual void Update(ZCamera* camera, float frameMix);
  ZShader* GetActiveShader() const { return shaders_[activeShaderIndex_]; }

protected:
  ZModel* model_ = nullptr;
  std::vector<ZShader*> shaders_;
};
