/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

   ZGrass.hpp

   Created by Adrian Sanchez on 09/08/2020.
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

#include "ZGameObject.hpp"

class ZTextureReadyEvent;

struct ZGrassUniforms
{
    glm::vec4 windDirection;
    float objectHeight;
    float timestamp;
    float windStrength;
};

struct ZGrass : public ZGameObject
{

public:

    ZGrass(unsigned int instances = 0);
    ~ZGrass() = default;

    void OnCreate() override;
    void OnDeserialize(const std::shared_ptr<ZOFObjectNode>& dataNode) override;
    void OnPrepare(double deltaTime) override;

    void TrimPatch(const glm::vec3& position, const glm::vec3& size);

    static constexpr unsigned int const& cPolygonCount = 3;

    ZHTexture texture;
    unsigned int instanceCount = 0;
    glm::vec3 windDirection;
    float objectHeight;
    float windStrength;

private:

    void UpdateVertexNormals(const ZHModel& model);
    void HandleTextureReady(const std::shared_ptr<ZTextureReadyEvent>& event);

private:

    float time_;
    std::shared_ptr<class ZGraphicsComponent> graphicsComp_;
    std::vector<ZHModel> polygons_;

};
