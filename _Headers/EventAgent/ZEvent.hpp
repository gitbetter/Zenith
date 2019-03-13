//
//  ZEvent.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 12/03/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#pragma once

// Includes
#include "ZCommon.hpp"

// Forward Declarations
//class SomeClass;

// Class and Data Structure Definitions
class ZEvent {

private:

public:

  virtual const ZEventType& EventType() const = 0;
  virtual float TimeStamp() const = 0;
  virtual void Serialize(std::ostringstream& out) const = 0;
  virtual std::shared_ptr<ZEvent> Copy() const = 0;
  virtual std::string Name() const = 0;

protected:

};

class ZBaseEvent : public ZEvent {

private:

  const float timeStamp_;

public:

  explicit ZBaseEvent(const float timeStamp = 0.f) : timeStamp_(timeStamp) { }
  virtual ~ZBaseEvent() { }

  virtual const ZEventType& EventType() const = 0;
  float TimeStamp() const override { return timeStamp_; }

  virtual void Serialize(std::ostringstream& out) const override { }

};
