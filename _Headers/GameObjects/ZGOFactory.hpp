//
//  ZGOFactory.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 18/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZCommon.hpp"

// Forward Declarations
class ZGameObject;
class ZComponent;

// Class and Data Structure Definitions
class ZGOFactory {
  using ZComponentCreator = std::shared_ptr<ZComponent> (ZGOFactory::*)(std::shared_ptr<ZGameObject>);
private:

public:

  ZGOFactory();
  ~ZGOFactory() { }

  ZGameObjectMap Create(ZOFTree* data);

  std::shared_ptr<ZComponent> CreateGraphicsComponent(std::shared_ptr<ZGameObject> gameObject);
  std::shared_ptr<ZComponent> CreateCameraComponent(std::shared_ptr<ZGameObject> gameObject);
  std::shared_ptr<ZComponent> CreatePhysicsComponent(std::shared_ptr<ZGameObject> gameObject);

protected:

  std::map<std::string, ZComponentCreator> componentCreators_;
};
