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

  std::string objectId_;

public:

  static const ZEventType Type;

  explicit ZFireEvent(const std::string& objectId = "") : objectId_(objectId) { }
  explicit ZFireEvent(std::istringstream& in) { in >> objectId_; }

  const std::string& ObjectID() const { return objectId_; }
  const ZEventType& EventType() const override { return Type; };
  std::shared_ptr<ZEvent> Copy() const override { return std::shared_ptr<ZFireEvent>(new ZFireEvent(objectId_)); }
  void Serialize(std::ostringstream& out) const override { out << objectId_; }
  std::string Name() const override { return "ZFireEvent"; }

protected:

};
