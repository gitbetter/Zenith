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

// Class and Data Structure Definitions
class ZGLGraphics : public ZGraphics {
private:
  static void GLFWErrorCallback(int id, const char* description);

  void Render(const std::vector<ZGameObject*>& gameObjects, float frameMix);

public:
  ZGLGraphics(int windowWidth, int windowHeight);
  ~ZGLGraphics();

  void Initialize(int windowWidth, int windowHeight);

  void Draw(const std::vector<ZGameObject*>& gameObjects, const std::vector<ZLight*>& gameLights, float frameMix) override;
  void SwapBuffers() override;
  void EnableStencilBuffer() override;
  void DisableStencilBuffer() override;
  void GenerateDepthMap() override;
  void DrawDepthMap(ZLight* light) override;
  void UpdateWindowSize() override;
  void Delete() override;
  ZWindow* GetWindow() override { return window_; }

protected:

};
