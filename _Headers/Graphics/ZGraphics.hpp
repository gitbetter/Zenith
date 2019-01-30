//
//  ZGraphics.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 27/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include <vector>

// Forward Declarations
class ZGameObject;
class ZWindow;

// Class and Data Structure Definitions
class ZGraphics {
private:
public:
  virtual ~ZGraphics() { }

  virtual void Draw(const std::vector<ZGameObject*>& gameObjects, float frameMix = 0.0) = 0;
  virtual void SwapBuffers() const = 0;
  virtual void UpdateWindowSize() { }
  virtual void Delete() = 0;
  virtual const ZWindow* GetWindow() = 0;
protected:
  ZWindow* window_ = nullptr;
};
