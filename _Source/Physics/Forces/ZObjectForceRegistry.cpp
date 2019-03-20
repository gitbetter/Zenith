/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZObjectForceRegistry.cpp

    Created by Adrian Sanchez on 14/02/2019.
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

#include "ZObjectForceRegistry.hpp"
#include "ZGameObject.hpp"
#include "ZForceGenerator.hpp"

void ZObjectForceRegistry::Add(ZGameObject* object, ZForceGenerator* generator) {
  ZObjectForceRegistration registration;
  registration.object = object;
  registration.forceGenerator = generator;
  registrations_.push_back(registration);
}

void ZObjectForceRegistry::Remove(ZGameObject* object, ZForceGenerator* generator) {
  registrations_.erase(std::remove_if(registrations_.begin(), registrations_.end(), [&](ZObjectForceRegistration registration) {
      return registration.object == object && registration.forceGenerator == generator;
  }), registrations_.end());
}

void ZObjectForceRegistry::Clear() {
  registrations_.clear();
}

void ZObjectForceRegistry::UpdateForces() {
  for (ZOFRegistry::iterator reg = registrations_.begin(); reg != registrations_.end(); reg++) {
    reg->forceGenerator->UpdateForce(reg->object);
  }
}
