//
//  ZQuitEvent.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 28/01/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#pragma once

// Includes
#include "ZEvent.hpp"
#include "ZObject.hpp"

// Forward Declarations

// Class and Data Structure Definitions
class ZFireEvent : public ZBaseEvent {

private:

  float x_, y_, z_;

public:

  static const ZEventType Type;

  explicit ZFireEvent(float x = 0.f, float y = 0.f, float z = 0.f) : x_(x), y_(y), z_(z) { }
  explicit ZFireEvent(std::istringstream& in) { in >> x_ >> y_ >> z_; }

  const ZEventType& EventType() const override { return Type; };
  std::shared_ptr<ZEvent> Copy() const override { return std::shared_ptr<ZFireEvent>(new ZFireEvent(x_, y_, z_)); }
  void Serialize(std::ostringstream& out) const override { out << x_ << " " << y_ << " " << z_; }
  std::string Name() const override { return "ZFireEvent"; }

  float X() const { return x_; }
  float Y() const { return y_; }
  float Z() const { return z_; }

protected:

};
