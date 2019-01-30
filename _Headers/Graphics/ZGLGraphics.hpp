//
//  ZGLGraphics.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 27/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZGraphics.hpp"
#include <vector>

// Forward Declarations
class ZGLWindow;
class ZGameObject;

// Class and Data Structure Definitions
class ZGLGraphics : public ZGraphics {
private:
  static void GLFWErrorCallback(int id, const char* description);

public:
  ZGLGraphics(int windowWidth, int windowHeight);
  virtual ~ZGLGraphics();

  void Initialize(int windowWidth, int windowHeight);

  virtual void Draw(const std::vector<ZGameObject*>& gameObjects, float frameMix) override;
  virtual void SwapBuffers() const override;
  virtual void UpdateWindowSize() override;
  virtual void Delete() override;
  virtual const ZWindow* GetWindow() override { return window_; }

protected:

};
