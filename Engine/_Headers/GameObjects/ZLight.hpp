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

// Class and Data Structure Definitions

struct ZLAttenuationProperties
{
    float constant{ 1.0f };
    float linear{ 0.7f };
    float quadratic{ 1.8f };
};

struct ZLSpotProperties
{
    glm::vec3 coneDirection{ 0.f, 1.f, 1.f };
    float cutoff;
    float exponent;
};

class ZLight : public ZGameObject
{

public:

    ZLightType type;
    bool enabled;
    glm::vec3 ambient{ 0.1f };
    glm::vec3 color{ 0.9f };
    ZLAttenuationProperties attenuation;
    ZLSpotProperties spot;

    ZLight(const glm::vec3& position = glm::vec3(0.f, 1.f, 0.f), const glm::quat& orientation = glm::quat(glm::vec3(0.f)))
        : ZGameObject(position, orientation), type(ZLightType::Point), enabled(true) { }
    ZLight(ZLightType lightType);
    ~ZLight() {}

    void Initialize() override { ZGameObject::Initialize(); }
    void Initialize(std::shared_ptr<ZOFNode> root) override;

    bool IsVisible() override { return true; }
    void Render(double deltaTime, const std::shared_ptr<ZShader>& shader, ZRenderOp renderOp = ZRenderOp::Color) override;

    std::shared_ptr<ZGameObject> Clone() override;

    std::vector<glm::mat4> LightSpaceMatrices() const { return lightspaceMatrices_; }
    const ZAABBox& LightSpaceRegion() const { return lightspaceRegion_; }
    const std::vector<float>& ShadowFarPlaneSplits() const { return shadowFarPlaneSplits_; }

    void UpdateLightspaceMatrices(const ZFrustum& frustum);

    DECLARE_OBJECT_CREATORS(ZLight)

private:

    std::vector<float> shadowFarPlaneSplits_;
    std::vector<glm::mat4> lightspaceMatrices_;
    ZAABBox lightspaceRegion_;

    static std::map<std::string, ZLightType> lightTypesMap;

};
