/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZTransformGizmo.cpp

    Created by Adrian Sanchez on 02/20/21.
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

#include "ZTransformGizmo.hpp"
#include "ZGameObject.hpp"
#include "ZPhysicsUniverse.hpp"
#include "ZGraphicsComponent.hpp"
#include "ZPhysicsComponent.hpp"
#include "ZColliderComponent.hpp"
#include "ZCylinder.hpp"
#include "ZScene.hpp"
#include "ZMaterial.hpp"
#include "ZShader.hpp"

void ZTransformGizmo::Initialize(const std::shared_ptr<ZScene>& scene)
{
    gizmo_ = std::make_shared<ZGameObject>();
    gizmo_->SetName("TransformGizmo");
    gizmo_->SetRenderOrder(ZRenderOrder::UI);

    axisArrows_[0] = CreateAxisArrow(glm::vec3(1.f, 0.f, 0.f));
    axisArrows_[1] = CreateAxisArrow(glm::vec3(0.f, 1.f, 0.f));
    axisArrows_[2] = CreateAxisArrow(glm::vec3(0.f, 0.f, 1.f));

    for (auto arrow : axisArrows_) {
        gizmo_->AddChild(arrow);
    }
    
    gizmo_->SetScale(glm::vec3(0.25f));
    gizmo_->SetOrientation(glm::vec3(0.f, 0.f, glm::radians(90.f)));
    gizmo_->SetPosition(glm::vec3(0.f, 0.5f, -1.f));
}

void ZTransformGizmo::Update()
{

}

void ZTransformGizmo::CleanUp()
{
    if (activeProjectScene_) {
        activeProjectScene_->RemoveGameObject(gizmo_);
    }
    for (auto arrow : axisArrows_) {
        gizmo_->RemoveChild(arrow);
        arrow->Destroy();
    }
    axisArrows_.fill(nullptr);
}

void ZTransformGizmo::OnProjectSceneChanged()
{
    if (previousActiveProjectScene_) {
        previousActiveProjectScene_->RemoveGameObject(gizmo_);
    }
    previousActiveProjectScene_ = activeProjectScene_;
    activeProjectScene_->AddGameObject(gizmo_);
}

std::shared_ptr<ZGameObject> ZTransformGizmo::CreateAxisArrow(const glm::vec3& axis)
{
    static std::shared_ptr<ZCylinder> cylinder = ZCylinder::Create(1.0f, 1.0f, 1.0f, glm::vec2(8.f));
    static std::shared_ptr<ZCylinder> cone = ZCylinder::Create(0.f, 0.5f, 1.0f, glm::vec2(8.f));
    static std::shared_ptr<ZShader> gizmoShader = ZShader::Create("/Shaders/Vertex/gizmo.vert", "/Shaders/Pixel/gizmo.frag");   // TODO: Unlocalize this shader

    ZMaterialProperties materialProps;
    materialProps.albedo = glm::vec4(glm::abs(axis), 1.f);
    auto arrowMaterial = ZMaterial::Create(materialProps);

    auto axisArrow = ZGameObject::Create();

    auto physicsComp = ZPhysicsComponent::CreateIn(axisArrow);
    auto colliderComp = ZColliderComponent::CreateIn(axisArrow);
    physicsComp->Initialize(
        ZPhysicsBodyType::Trigger,
        0.f
    );
    colliderComp->Initialize(
        ZColliderType::Capsule,
        glm::vec3(0.12f, 5.1f, 0.f),
        glm::vec3(0.f, axis.y > 0.f || axis.z > 0.f ? -2.6f : 2.6f, 0.f)
    );

    auto arrowBase = ZGameObject::Create(
        glm::vec3(0.f, 0.f, 2.5f),
        glm::vec3(0.f),
        glm::vec3(0.03f, 0.03f, 5.f)
    );
    auto graphicsComp = ZGraphicsComponent::CreateIn(arrowBase);
    graphicsComp->Initialize(cylinder, gizmoShader);
    graphicsComp->AddMaterial(arrowMaterial);

    auto arrowTop = ZGameObject::Create(
        glm::vec3(0.f, 0.f, 5.f),
        glm::vec3(0.f),
        glm::vec3(0.5f, 0.5f, 0.5f)
    );
    graphicsComp = ZGraphicsComponent::CreateIn(arrowTop);
    graphicsComp->Initialize(cone, gizmoShader);
    graphicsComp->AddMaterial(arrowMaterial);

    axisArrow->AddChild(arrowBase);
    axisArrow->AddChild(arrowTop);

    axisArrow->SetOrientation(axis * PI * 0.5f);

    return axisArrow;
}
