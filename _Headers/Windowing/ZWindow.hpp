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

  virtual void CloseWindow() const = 0;
  virtual bool WindowShouldClose() const = 0;
  virtual void PollEvents() const = 0;
  virtual void Destroy() = 0;
  virtual void SetSize(int width, int height);

  int GetWidth() const { return width_; }
  int GetHeight() const { return height_; }

protected:
  int width_, height_;
};
