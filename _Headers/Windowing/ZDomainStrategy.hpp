//
//  ZDomainStrategy.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 07/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include <glm/glm.hpp>

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
class ZDomainStrategy {
private:

public:

  ZDomainStrategy() { }
  virtual ~ZDomainStrategy() { }

  virtual void Initialize() = 0;
  virtual void CreateWindow(int width, int heights) = 0;
  virtual void PollEvents() = 0;
  virtual void CaptureCursor() = 0;
  virtual void ReleaseCursor() = 0;
  virtual void Resize(int width, int height) = 0;
  virtual glm::vec2 FramebufferSize() = 0;
  virtual bool IsWindowClosing() = 0;
  virtual void CloseWindow() = 0;
  virtual void CleanUp() = 0;

protected:

};
