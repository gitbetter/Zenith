//
//  ZIUpDownCommand.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#pragma once

// Includes
#include "ZEvent.hpp"

// Forward Declarations

// Class and Data Structure Definitions
class ZObjectLookEvent : public ZBaseEvent {

private:

  float yaw_, pitch_;

public:

  static const ZEventType Type;

  explicit ZObjectLookEvent(float yaw = 0.f, float pitch = 0.f) : yaw_(yaw), pitch_(pitch) { }
  explicit ZObjectLookEvent(std::istringstream& in) { in >> yaw_; in >> pitch_; }

  const ZEventType& EventType() const override { return Type; };
  std::shared_ptr<ZEvent> Copy() const override { return std::shared_ptr<ZObjectLookEvent>(new ZObjectLookEvent(yaw_, pitch_)); }
  void Serialize(std::ostringstream& out) const override { out << pitch_ << " " << yaw_; }
  std::string Name() const override { return "ZObjectLookEvent"; }

  float Yaw() const { return yaw_; }
  float Pitch() const { return pitch_; }

protected:

};
