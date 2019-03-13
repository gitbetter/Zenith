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

// Forward Declarations

// Class and Data Structure Definitions
class ZFireEvent : public ZBaseEvent {

private:

public:

  static const ZEventType Type;

  ZFireEvent() { }

  const ZEventType& EventType() const override { return Type; };
  std::shared_ptr<ZEvent> Copy() const override { return std::shared_ptr<ZFireEvent>(new ZFireEvent); }
  std::string Name() const override { return "ZFireEvent"; }

protected:

};
