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
  ZDomain(unsigned int windowWidth, unsigned int windowHeight) : windowWidth_(windowWidth), windowHeight_(windowHeight) { }
  virtual ~ZDomain() { }

  void Initialize();

  unsigned int WindowWidth() const { return windowWidth_; }
  unsigned int WindowHeight() const { return windowHeight_; }
  unsigned int ResolutionX() const { return resolutionX_; }
  unsigned int ResolutionY() const { return resolutionY_; }

  ZDomainStrategy* Strategy() { return domainStrategy_; }

  void ResizeWindow(int width, int height);

  void CleanUp();

protected:
  ZDomainStrategy* domainStrategy_ = nullptr;
  unsigned int windowWidth_, windowHeight_;
  unsigned int resolutionX_, resolutionY_;
};
