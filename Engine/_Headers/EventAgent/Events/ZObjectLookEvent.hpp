/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZIUpDownCommand.hpp

    Created by Adrian Sanchez on 28/01/2019.
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
#include "ZEvent.hpp"

// Forward Declarations

// Class and Data Structure Definitions
class ZObjectLookEvent : public ZEvent
{

private:

    float yaw_, pitch_;

public:

    static const ZEventType Type;

    explicit ZObjectLookEvent(float yaw = 0.f, float pitch = 0.f) : yaw_(yaw), pitch_(pitch) {}
    explicit ZObjectLookEvent(std::istringstream& in) { in >> yaw_; in >> pitch_; }

    const ZEventType& EventType() const override { return Type; };
    std::shared_ptr<ZEvent> Copy() const override { return std::shared_ptr<ZObjectLookEvent>(new ZObjectLookEvent(yaw_, pitch_)); }
    void Serialize(std::ostringstream& out) const override { out << pitch_ << " " << yaw_; }
    std::string Name() const override { return "ZObjectLookEvent"; }

    float Yaw() const { return yaw_; }
    float Pitch() const { return pitch_; }

protected:

};
