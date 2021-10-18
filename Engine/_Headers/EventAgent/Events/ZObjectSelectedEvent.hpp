/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZObjectSelectedEvent.hpp

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

#include "ZEvent.hpp"

class ZObjectSelectedEvent : public ZEvent
{

private:

    ZHGameObject object_;
    glm::vec3 position_;

public:

    static const ZTypeIdentifier Type;

    explicit ZObjectSelectedEvent(const ZHGameObject& object, const glm::vec3& pos) : object_(object), position_(pos) {}
    explicit ZObjectSelectedEvent(std::istringstream& in) { /* in >> object_; */ }

    const ZTypeIdentifier& EventType() const override { return Type; };
    std::shared_ptr<ZEvent> Copy() const override { return std::shared_ptr<ZObjectSelectedEvent>(new ZObjectSelectedEvent(object_, position_)); }
    void Serialize(std::ostringstream& out) const override { out << object_; }
    std::string Name() const override { return "ZObjectSelectedEvent"; }

    const ZHGameObject& Object() const { return object_; }
    const glm::vec3& Position() const { return position_; }

};
