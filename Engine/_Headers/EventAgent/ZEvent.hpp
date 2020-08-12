/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZEvent.hpp

    Created by Adrian Sanchez on 12/03/2019.
    Copyright © 2019 Pervasive Sense. All rights reserved.

  This file is part of Zenith.

  Zenith is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Zenith is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Zenith.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

// Includes
#include "ZEngine.hpp"

// Forward Declarations
//class SomeClass;

// Class and Data Structure Definitions
class ZEvent
{

private:

public:

    virtual const ZEventType& EventType() const = 0;
    virtual float TimeStamp() const = 0;
    virtual void SetTimeStamp(float timestamp) = 0;
    virtual void Serialize(std::ostringstream& out) const = 0;
    virtual std::shared_ptr<ZEvent> Copy() const = 0;
    virtual std::string Name() const = 0;

protected:

};

class ZBaseEvent : public ZEvent
{

private:

    float timeStamp_;

public:

    explicit ZBaseEvent(const float timeStamp = 0.f) : timeStamp_(timeStamp) {}
    virtual ~ZBaseEvent() {}

    virtual const ZEventType& EventType() const override = 0;
    float TimeStamp() const override { return timeStamp_; }

    void SetTimeStamp(float timestamp) override { timeStamp_ = timestamp; }

    virtual void Serialize(std::ostringstream& out) const override {}

};
