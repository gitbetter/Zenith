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

  float x_, y_, z_;

public:

  static const ZEventType Type;

  explicit ZObjectDragEvent(float x = 0.f, float y = 0.f, float z = 0.f) : x_(x), y_(y), z_(z) { }
  explicit ZObjectDragEvent(std::istringstream& in) { in >> x_; in >> y_; in >> z_; }

  const ZEventType& EventType() const override { return Type; };
  std::shared_ptr<ZEvent> Copy() const override { return std::shared_ptr<ZObjectDragEvent>(new ZObjectDragEvent(x_, y_, z_)); }
  void Serialize(std::ostringstream& out) const override { out << x_ << " " << y_ << " " << z_; }
  std::string Name() const override { return "ZObjectDragEvent"; }

  float DeltaX() const { return x_; }
  float DeltaY() const { return y_; }
  float DeltaZ() const { return z_; }

protected:

};
