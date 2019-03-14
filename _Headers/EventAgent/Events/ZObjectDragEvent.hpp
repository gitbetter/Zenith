//
//  ZObjectDragEvent.hpp
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
class ZObjectDragEvent : public ZBaseEvent {

private:

  glm::vec3 delta_;

public:

  static const ZEventType Type;

  explicit ZObjectDragEvent(glm::vec3 delta = glm::vec3(0.f)) : delta_(delta) { }
  explicit ZObjectDragEvent(std::istringstream& in) { in >> delta_.x; in >> delta_.y; in >> delta_.z; }

  const ZEventType& EventType() const override { return Type; };
  std::shared_ptr<ZEvent> Copy() const override { return std::shared_ptr<ZObjectDragEvent>(new ZObjectDragEvent(delta_)); }
  void Serialize(std::ostringstream& out) const override { out << delta_.x << " " << delta_.y << " " << delta_.z; }
  std::string Name() const override { return "ZObjectDragEvent"; }

  glm::vec3 Delta() const { return delta_; }

protected:

};
