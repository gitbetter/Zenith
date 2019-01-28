//
//  ZEngine.hpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes

// Forward Declarations
class ZGraphics;

// Class Definitions
class ZEngine {
private:
  static ZGraphics* graphics_;
public:
  static const float MS_PER_UPDATE;
  static const int MAX_FIXED_UPDATE_ITERATIONS;

  static ZGraphics* GetGraphics();

  static void Provide(ZGraphics& graphics);
  // More provide overloads for different engine subsystems (i.e. physics, audio, input, etc.)
protected:
};
