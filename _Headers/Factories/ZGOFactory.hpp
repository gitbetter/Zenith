/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZGOFactory.hpp

    Created by Adrian Sanchez on 18/02/2019.
    Copyright © 2019 Pervasive Sense. All rights reserved.

  This file is part of Zenith.

  Zenith is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Zenith is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Zenith.  If not, see <https://www.gnu.org/licenses/>.
*/

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

  ZGameObjectMap Create(std::shared_ptr<ZOFTree> data);

  std::shared_ptr<ZComponent> CreateGraphicsComponent(std::shared_ptr<ZGameObject> gameObject);
  std::shared_ptr<ZComponent> CreateCameraComponent(std::shared_ptr<ZGameObject> gameObject);
  std::shared_ptr<ZComponent> CreatePhysicsComponent(std::shared_ptr<ZGameObject> gameObject);

protected:

  std::map<std::string, ZComponentCreator> componentCreators_;

};
