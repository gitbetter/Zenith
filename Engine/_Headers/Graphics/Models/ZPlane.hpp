/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZPlane.hpp

    Created by Adrian Sanchez on 02/19/21.
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
#include "ZModel.hpp"

// Forward Declarations

// Class Definitions
class ZPlane : public ZModel
{

public:

    ZPlane(const glm::vec2& size = glm::vec2(1.f))
        : ZModel(), size_(size)
    { };
    virtual ~ZPlane() {}

    void Initialize() override;
    void Initialize(const std::shared_ptr<ZOFNode>& data) override;

    static std::shared_ptr<ZPlane> Create(const glm::vec2& size);

protected:

    glm::vec2 size_;

};
