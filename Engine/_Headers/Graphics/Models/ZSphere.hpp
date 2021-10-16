/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZSphere.hpp

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

#include "ZModel.hpp"

struct ZSphere : public ZModel
{

    ZSphere(float radius = 1.f, const glm::vec2& segments = glm::vec2(16.f), bool smooth = true)
        : ZModel(), radius_(radius), segments_(segments), smooth_(smooth)
    { };
    virtual ~ZSphere() = default;

    void OnCreate() override;
    void OnDeserialize(const std::shared_ptr<ZOFObjectNode>& dataNode) override;

protected:

    float radius_;
    glm::vec2 segments_;
    bool smooth_;

    void BuildSmooth();
    void BuildFlat();

};
