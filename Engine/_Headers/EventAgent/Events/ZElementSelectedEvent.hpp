/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZElementSelectedEvent.hpp

    Created by Adrian Sanchez on 10/21/2021.
    Copyright © 2021 Pervasive Sense. All rights reserved.

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

class ZElementSelectedEvent : public ZEvent
{

private:

    ZHUIElement element_;
    glm::vec3 position_;

public:

    static const ZTypeIdentifier Type;

    explicit ZElementSelectedEvent(const ZHUIElement& element, const glm::vec3& pos) : element_(element), position_(pos) {}
    explicit ZElementSelectedEvent(std::istringstream& in) { /* in >> element_; */ }

    const ZTypeIdentifier& EventType() const override { return Type; };
    std::shared_ptr<ZEvent> Copy() const override { return std::shared_ptr<ZElementSelectedEvent>(new ZElementSelectedEvent(element_, position_)); }
    void Serialize(std::ostringstream& out) const override { out << element_; }
    std::string Name() const override { return "ZElementSelectedEvent"; }

    const ZHUIElement& Element() const { return element_; }
    const glm::vec3& Position() const { return position_; }

};
