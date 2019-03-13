//
//  ZInput.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#pragma once

// Includes

// Forward Declarations
class ZObject;

// Class and Data Structure Definitions
class ZInput {

private:

public:

  virtual ~ZInput() { }

  virtual void Process() = 0;
  virtual void GetPointerPosition(double& x, double& y) = 0;

protected:

};
