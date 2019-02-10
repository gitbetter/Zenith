//
//  ZDomain.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 27/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
// #include <iostream>

// Forward Declarations
class ZDomainStrategy;

// Class and Data Structure Definitions
class ZDomain {
private:

public:
  ZDomain(unsigned int windowWidth, unsigned int windowHeight) : width_(windowWidth), height_(windowHeight) { }
  virtual ~ZDomain() { }

  void Initialize();

  unsigned int WindowWidth() const { return width_; }
  unsigned int WindowHeight() const { return height_; }

  ZDomainStrategy* Strategy() { return domainStrategy_; }

  void ResizeWindow(int width, int height);

  void CleanUp();

protected:
  ZDomainStrategy* domainStrategy_;
  unsigned int width_, height_;
};
