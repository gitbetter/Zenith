/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZRaycastEvent.hpp

    Created by Adrian Sanchez on 13/03/2019.
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
#include "ZEvent.hpp"

// Forward Declarations

// Class and Data Structure Definitions
class ZRaycastEvent : public ZBaseEvent {

private:

  glm::vec3 origin_;
  glm::vec3 direction_;

public:

  static const ZEventType Type;

  explicit ZRaycastEvent(glm::vec3 origin = glm::vec3(0.f), glm::vec3 direction = glm::vec3(0.f)) : origin_(origin), direction_(direction) { }
  explicit ZRaycastEvent(std::istringstream& in)
  { in >> origin_.x; in >> origin_.y; in >> origin_.z;
    in >> direction_.x; in >> direction_.y; in >> direction_.z; }

  const ZEventType& EventType() const override { return Type; };
  std::shared_ptr<ZEvent> Copy() const override { return std::shared_ptr<ZRaycastEvent>(new ZRaycastEvent(origin_, direction_)); }
  void Serialize(std::ostringstream& out) const override
  { out << origin_.x << " " << origin_.y << " " << origin_.z << direction_.x << " " << direction_.y << " " << direction_.z; }
  std::string Name() const override { return "ZRaycastEvent"; }

  glm::vec3 Origin() const { return origin_; }
  glm::vec3 Direction() const { return direction_; }

protected:

};
