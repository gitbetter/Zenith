//
//  ZRaycastEvent.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 13/03/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

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
