//
//  ZCollisionComponent.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 23/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZComponent.hpp"
#include "ZCollisionPrimitive.hpp"

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
class ZCollisionComponent : public ZComponent {

public:

  ~ZCollisionComponent() { }

  void Initialize(ZOFNode* root) override;

  void Update() override;

  void SetCollisionPrimitive(ZCollisionPrimitive* primitive) { collisionPrimitive_ = primitive; }

  ZCollisionPrimitive* CollisionPrimitive() { return collisionPrimitive_; }

protected:

  ZCollisionPrimitive* collisionPrimitive_ = nullptr;

};
