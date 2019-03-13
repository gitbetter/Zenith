//
//  ZObjectMoveEvent.hpp
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
class ZObjectMoveEvent : public ZBaseEvent {

private:

  float x_, y_, z_;

public:

  static const ZEventType Type;

  explicit ZObjectMoveEvent(float x = 0.f, float y = 0.f, float z = 0.f) : x_(x), y_(y), z_(z) { }
  explicit ZObjectMoveEvent(std::istringstream& in) { in >> x_; in >> y_; in >> z_; }

  const ZEventType& EventType() const override { return Type; };
  std::shared_ptr<ZEvent> Copy() const override { return std::shared_ptr<ZObjectMoveEvent>(new ZObjectMoveEvent(x_, y_, z_)); }
  void Serialize(std::ostringstream& out) const override { out << z_ << " " << y_ << " " << x_; }
  std::string Name() const override { return "ZObjectMoveEvent"; }

  float X() const { return z_; }
  float Y() const { return y_; }
  float Z() const { return z_; }

protected:

};
