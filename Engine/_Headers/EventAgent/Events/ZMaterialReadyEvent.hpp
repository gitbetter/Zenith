/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZMaterialReadyEvent.hpp

    Created by Adrian Sanchez on 31/01/2021.
    Copyright � 2019 Pervasive Sense. All rights reserved.

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
class ZMaterialReadyEvent : public ZEvent
{

private:

    std::shared_ptr<ZMaterial> material_;

public:

    static const ZTypeIdentifier Type;

    explicit ZMaterialReadyEvent(const std::shared_ptr<ZMaterial>& material) : material_(material) {}
    explicit ZMaterialReadyEvent(std::istringstream& in) {}

    const ZTypeIdentifier& EventType() const override { return Type; };
    std::shared_ptr<ZEvent> Copy() const override { return std::shared_ptr<ZMaterialReadyEvent>(new ZMaterialReadyEvent(material_)); }
    void Serialize(std::ostringstream& out) const override {}
    std::string Name() const override { return "ZMaterialReadyEvent"; }

    std::shared_ptr<ZMaterial> Material() { return material_; }

};
