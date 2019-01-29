//
//  ZWindow.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 27/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
// #include <iostream>

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
class ZWindow {
private:

public:
  ZWindow(int windowWidth, int windowHeight) : width_(windowWidth), height_(windowHeight) { }
  virtual ~ZWindow() { }

  virtual bool WindowShouldClose() const = 0;
  virtual void PollEvents() const = 0;
  virtual void SwapBuffers() const = 0;
  virtual void Destroy() = 0;
protected:
  int width_, height_;
};
