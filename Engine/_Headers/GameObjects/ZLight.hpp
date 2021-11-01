/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

   ZLight.hpp

   Created by Adrian Sanchez on 01/02/2019.
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
#include "ZFrustum.hpp"

class ZScene;
class ZRenderStateGroup;

struct ZLight : public ZGameObject
{

public:

    ZLight(ZLightType lightType);
    ~ZLight() = default;

    virtual void OnCreate() override;
    virtual void OnDeserialize(const std::shared_ptr<ZOFObjectNode>& dataNode) override;
    virtual void OnUpdate(double deltaTime) override;
    virtual void OnCloned(ZGameObject* original) override;

    void UpdateLightspaceMatrices(const ZFrustum& frustum);

public:

    ZLightType lightType;
    Light lightProperties;
    glm::vec4 shadowFarPlaneSplits;
    std::vector<glm::mat4> lightspaceMatrices;
    ZAABBox lightspaceRegion;

private:

    static std::map<std::string, ZLightType> lightTypesMap;
    static ZIDSequence idGenerator_;

};
