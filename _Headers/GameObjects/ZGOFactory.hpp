//
//  ZGOFactory.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 18/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZOFTree.hpp"

// Forward Declarations
class ZGameObject;
class ZComponent;

// Class and Data Structure Definitions
class ZGOFactory {
  typedef ZComponent* (ZGOFactory::*ZComponentCreator)();
private:
public:
  ZGOFactory();
  ~ZGOFactory() { }

  std::map<std::string, ZGameObject*> Create(ZOFTree* data);

  ZComponent* CreateGraphicsComponent();
  ZComponent* CreateCameraComponent();
  ZComponent* CreatePhysicsComponent();

protected:
  std::map<std::string, ZComponentCreator> componentCreators_;
};
