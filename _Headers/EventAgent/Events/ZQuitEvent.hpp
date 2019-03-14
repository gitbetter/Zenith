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
class ZQuitEvent : public ZBaseEvent {

private:

public:

  static const ZEventType Type;

  ZQuitEvent() { }

  const ZEventType& EventType() const override { return Type; };
  std::shared_ptr<ZEvent> Copy() const override { return std::shared_ptr<ZQuitEvent>(new ZQuitEvent); }
  std::string Name() const override { return "ZQuitEvent"; }

protected:

};
