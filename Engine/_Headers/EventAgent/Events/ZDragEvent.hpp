/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZDragEvent.hpp

    Created by Adrian Sanchez on 13/03/2019.
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
class ZDragEvent : public ZEvent
{

private:

    float x_, y_, z_;

public:

    static const ZTypeIdentifier Type;

    explicit ZDragEvent(float x = 0.f, float y = 0.f, float z = 0.f) : x_(x), y_(y), z_(z) {}
    explicit ZDragEvent(std::istringstream& in) { in >> x_; in >> y_; in >> z_; }

    const ZTypeIdentifier& EventType() const override { return Type; };
    std::shared_ptr<ZEvent> Copy() const override { return std::shared_ptr<ZDragEvent>(new ZDragEvent(x_, y_, z_)); }
    void Serialize(std::ostringstream& out) const override { out << x_ << " " << y_ << " " << z_; }
    std::string Name() const override { return "ZDragEvent"; }

    float DeltaX() const { return x_; }
    float DeltaY() const { return y_; }
    float DeltaZ() const { return z_; }

protected:

};
