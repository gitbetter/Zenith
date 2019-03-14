//
//  ZObjectSelectedEvent.hpp
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
class ZObjectSelectedEvent : public ZBaseEvent {

private:

  std::string objectId_;

public:

  static const ZEventType Type;

  explicit ZObjectSelectedEvent(const std::string& objectId) : objectId_(objectId) { }
  explicit ZObjectSelectedEvent(std::istringstream& in) { in >> objectId_; }

  const ZEventType& EventType() const override { return Type; };
  std::shared_ptr<ZEvent> Copy() const override { return std::shared_ptr<ZObjectSelectedEvent>(new ZObjectSelectedEvent(objectId_)); }
  void Serialize(std::ostringstream& out) const override { out << objectId_; }
  std::string Name() const override { return "ZObjectSelectedEvent"; }

  const std::string& ObjectID() const { return objectId_; }

protected:

};
