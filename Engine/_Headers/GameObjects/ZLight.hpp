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

// Includes
#include "ZGameObject.hpp"
#include "ZFrustum.hpp"

// Forward Declarations
class ZScene;
class ZRenderStateGroup;

// Class and Data Structure Definitions

class ZLight : public ZGameObject
{

public:

    ZLightType type;
    Light properties;

    ZLight(const glm::vec3& position = glm::vec3(0.f, 1.f, 0.f), const glm::quat& orientation = glm::quat(glm::vec3(0.f)), const glm::vec3& scale = glm::vec3(1.f));
    ZLight(ZLightType lightType);
    ~ZLight() {}

    void Initialize() override;
    void Initialize(std::shared_ptr<ZOFNode> root) override;

    bool IsVisible() override { return true; }
    void Prepare(double deltaTime) override;

    std::shared_ptr<ZGameObject> Clone() override;

    std::vector<glm::mat4> LightSpaceMatrices() const { return lightspaceMatrices_; }
    const ZAABBox& LightSpaceRegion() const { return lightspaceRegion_; }
    const glm::vec4 ShadowFarPlaneSplits() const { return shadowFarPlaneSplits_; }
    const std::shared_ptr<ZRenderStateGroup> RenderState() const { return renderState_; }

    void UpdateLightspaceMatrices(const ZFrustum& frustum);

    DECLARE_OBJECT_CREATORS(ZLight)

private:

    glm::vec4 shadowFarPlaneSplits_;
    std::vector<glm::mat4> lightspaceMatrices_;
    ZAABBox lightspaceRegion_;

    std::shared_ptr<ZRenderStateGroup> renderState_;
    std::shared_ptr<ZUniformBuffer> uniformBuffer_;

    static std::map<std::string, ZLightType> lightTypesMap;

};
