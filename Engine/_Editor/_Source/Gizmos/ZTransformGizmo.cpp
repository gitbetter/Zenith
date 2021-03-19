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
#include "ZServices.hpp"
#include "ZGameObject.hpp"
#include "ZPhysicsUniverse.hpp"
#include "ZGraphicsComponent.hpp"
#include "ZPhysicsComponent.hpp"
#include "ZColliderComponent.hpp"
#include "ZCylinder.hpp"
#include "ZScene.hpp"
#include "ZMaterial.hpp"
#include "ZShader.hpp"
#include "ZCamera.hpp"

void ZTransformGizmo::Initialize(const std::shared_ptr<ZScene>& scene)
{
    ZEditorGizmo::Initialize(scene);

    gizmo_->SetName("TransformGizmo");

    axisArrows_[0] = CreateAxisArrow(glm::vec3(1.f, 0.f, 0.f));
    axisArrows_[1] = CreateAxisArrow(glm::vec3(0.f, 1.f, 0.f));
    axisArrows_[2] = CreateAxisArrow(glm::vec3(0.f, 0.f, 1.f));

    for (auto arrow : axisArrows_) {
        gizmo_->AddChild(arrow);
    }
    
    gizmo_->SetOrientation(glm::vec3(0.f, 0.f, glm::radians(90.f)));
}

void ZTransformGizmo::Update()
{
    if (!activeProjectScene_) return;

    if (auto cam = activeProjectScene_->ActiveCamera()) {
        auto cameraDist = (glm::length(gizmo_->Position() - activeProjectScene_->ActiveCamera()->Position()));
        auto worldSize = (2.f * glm::tan(cam->Frustum().fov * 0.5f)) * cameraDist;
        gizmo_->SetScale(glm::vec3(worldSize * 0.05f));
    }
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

bool ZTransformGizmo::TryActivate(const ZRect& viewportRect)
{
    auto cursorPos = ZServices::Input()->GetCursorPosition() - viewportRect.position;
    auto ray = activeProjectScene_->ScreenPointToWorldRay(cursorPos, viewportRect.size);

    for (int i = 0; i < 3; i++) {
        for (auto child : axisArrows_[i]->Children()) {
            auto graphics = child->FindComponent<ZGraphicsComponent>();
            auto aabb = graphics->AABB();
            if (aabb.Intersects(ray)) {
                auto axisRay = ZRay(gizmo_->Position(), glm::eulerAngles(axisArrows_[i]->Orientation()));
                previousAxisPoint_ = axisRay.ClosestPointTo(ray);
                previousCursorPos_ = cursorPos;
                active_ = true;
                selectedAxis_ = i;
                return true;
            }
        }
    }

    Deactivate();
    return false;
}

void ZTransformGizmo::OnProjectSceneChanged()
{
    if (previousActiveProjectScene_) {
        previousActiveProjectScene_->RemoveGameObject(gizmo_);
    }
    previousActiveProjectScene_ = activeProjectScene_;
    activeProjectScene_->AddGameObject(gizmo_);
}

void ZTransformGizmo::Manipulate(const ZRect& viewportRect, glm::mat4& transform)
{
    if (active_) {
        auto cursorPos = ZServices::Input()->GetCursorPosition() - viewportRect.position;
        auto ray = activeProjectScene_->ScreenPointToWorldRay(cursorPos, viewportRect.size);

        auto cursorDelta = glm::abs(cursorPos - previousCursorPos_);
        bool cursorWrapped = cursorDelta.x >= viewportRect.size.x * 0.95f || cursorDelta.y >= viewportRect.size.y * 0.95f;
        previousCursorPos_ = cursorPos;

        auto axisArrow = axisArrows_[selectedAxis_];
        auto axisRay = ZRay(gizmo_->Position(), glm::eulerAngles(axisArrow->Orientation()));

        glm::vec3 currentAxisPoint = axisRay.ClosestPointTo(ray);
        glm::vec3 delta = (currentAxisPoint - previousAxisPoint_) * sensitivity_;
        previousAxisPoint_ = currentAxisPoint;

        if (!cursorWrapped)
            gizmo_->Translate(-delta, true);

        transform[3] = gizmo_->ModelMatrix()[3];
    }
}

void ZTransformGizmo::Deactivate()
{
    active_ = false;
    selectedAxis_ = -1;
    previousAxisPoint_ = glm::vec3(0.f);
}

std::shared_ptr<ZGameObject> ZTransformGizmo::CreateAxisArrow(const glm::vec3& axis)
{
    static std::shared_ptr<ZCylinder> cylinder = ZCylinder::Create(1.0f, 1.0f, 1.0f, glm::vec2(8.f));
    static std::shared_ptr<ZCylinder> cone = ZCylinder::Create(0.f, 0.5f, 1.0f, glm::vec2(8.f));

    ZMaterialProperties materialProps;
    materialProps.look.albedo = glm::vec4(glm::abs(axis), 1.f);
    auto arrowMaterial = ZMaterial::Create(materialProps, ZShader::Create("/Shaders/Vertex/gizmo.vert", "/Shaders/Pixel/gizmo.frag"));

    auto axisArrow = ZGameObject::Create();

    auto arrowBase = ZGameObject::Create(
        glm::vec3(0.f, 0.f, 2.5f),
        glm::vec3(0.f),
        glm::vec3(0.03f, 0.03f, 5.f)
    );
    arrowBase->SetRenderOrder(ZRenderLayer::UI);
    auto graphicsComp = ZGraphicsComponent::CreateIn(arrowBase);
    graphicsComp->Initialize(cylinder);
    graphicsComp->DisableShadowCasting();
    graphicsComp->DisableDepthInfo();
    graphicsComp->AddMaterial(arrowMaterial);

    auto arrowTop = ZGameObject::Create(
        glm::vec3(0.f, 0.f, 5.f),
        glm::vec3(0.f),
        glm::vec3(0.5f, 0.5f, 0.5f)
    );
    arrowTop->SetRenderOrder(ZRenderLayer::UI);
    graphicsComp = ZGraphicsComponent::CreateIn(arrowTop);
    graphicsComp->Initialize(cone);
    graphicsComp->DisableShadowCasting();
    graphicsComp->DisableDepthInfo();
    graphicsComp->AddMaterial(arrowMaterial);

    axisArrow->AddChild(arrowBase);
    axisArrow->AddChild(arrowTop);

    axisArrow->SetOrientation(axis * PI * 0.5f);

    return axisArrow;
}
