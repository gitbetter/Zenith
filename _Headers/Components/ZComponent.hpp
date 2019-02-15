//
//  ZComponent.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes

// Forward Declarations
class ZGameObject;

// Class and Data Structure Definitions
class ZComponent {
friend class ZGameObject;
private:
public:
  virtual ~ZComponent() { }

  ZGameObject* Object() { return object_; }

protected:
  ZGameObject* object_;
};
