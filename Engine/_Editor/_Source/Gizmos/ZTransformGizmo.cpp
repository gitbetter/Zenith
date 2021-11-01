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

    ZServices::GameObjectManager()->SetName(gizmo_, "TransformGizmo");

    axisArrows_[0] = CreateAxisArrow(glm::vec3(1.f, 0.f, 0.f));
    axisArrows_[1] = CreateAxisArrow(glm::vec3(0.f, 1.f, 0.f));
    axisArrows_[2] = CreateAxisArrow(glm::vec3(0.f, 0.f, 1.f));

    for (auto arrow : axisArrows_)
    {
        ZServices::GameObjectManager()->AddChild(gizmo_, arrow);
    }
    
    ZServices::GameObjectManager()->SetOrientation(gizmo_, glm::vec3(0.f, 0.f, glm::radians(90.f)));
}

void ZTransformGizmo::Update()
{
    if (!activeProjectScene_) return;

    if (const ZHGameObject cam = activeProjectScene_->ActiveCamera())
    {
        ZCamera* camObj = ZServices::GameObjectManager()->Dereference<ZCamera>(cam);
        auto cameraDist = (glm::length(ZServices::GameObjectManager()->Position(gizmo_) - ZServices::GameObjectManager()->Position(activeProjectScene_->ActiveCamera())));
        auto worldSize = (2.f * glm::tan(camObj->frustum.fov * 0.5f)) * cameraDist;
        ZServices::GameObjectManager()->SetScale(gizmo_, glm::vec3(worldSize * 0.05f));
    }
}

void ZTransformGizmo::CleanUp()
{
    if (activeProjectScene_)
    {
        activeProjectScene_->RemoveGameObject(gizmo_);
    }
    for (auto arrow : axisArrows_)
    {
        ZServices::GameObjectManager()->RemoveChild(gizmo_, arrow);
        ZServices::GameObjectManager()->Destroy(arrow);
    }
    axisArrows_.fill(ZHGameObject());
}

bool ZTransformGizmo::TryActivate(const ZRect& viewportRect)
{
    auto cursorPos = ZServices::Input()->GetCursorPosition() - viewportRect.position;
    auto ray = activeProjectScene_->ScreenPointToWorldRay(cursorPos, viewportRect.size);

    for (int i = 0; i < 3; i++)
    {
        for (auto child : ZServices::GameObjectManager()->Children(axisArrows_[i]))
        {
            const ZHComponent graphics = ZServices::GameObjectManager()->FindComponent<ZGraphicsComponent>(child);
            ZGraphicsComponent* graphicsObj = ZServices::ComponentManager()->Dereference<ZGraphicsComponent>(graphics);
            auto aabb = graphicsObj->bounds;
            if (aabb.Intersects(ray))
            {
                auto axisRay = ZRay(ZServices::GameObjectManager()->Position(gizmo_), glm::eulerAngles(ZServices::GameObjectManager()->Orientation(axisArrows_[i])));
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
    if (active_)
    {
        auto cursorPos = ZServices::Input()->GetCursorPosition() - viewportRect.position;
        auto ray = activeProjectScene_->ScreenPointToWorldRay(cursorPos, viewportRect.size);

        auto cursorDelta = glm::abs(cursorPos - previousCursorPos_);
        bool cursorWrapped = cursorDelta.x >= viewportRect.size.x * 0.95f || cursorDelta.y >= viewportRect.size.y * 0.95f;
        previousCursorPos_ = cursorPos;

        auto axisArrow = axisArrows_[selectedAxis_];
        auto axisRay = ZRay(ZServices::GameObjectManager()->Position(gizmo_), glm::eulerAngles(ZServices::GameObjectManager()->Orientation(axisArrow)));

        glm::vec3 currentAxisPoint = axisRay.ClosestPointTo(ray);
        glm::vec3 delta = (currentAxisPoint - previousAxisPoint_) * sensitivity_;
        previousAxisPoint_ = currentAxisPoint;

        if (!cursorWrapped)
        {
            ZServices::GameObjectManager()->Translate(gizmo_, -delta, true);
        }

        transform[3] = ZServices::GameObjectManager()->ModelMatrix(gizmo_)[3];
    }
}

void ZTransformGizmo::Deactivate()
{
    active_ = false;
    selectedAxis_ = -1;
    previousAxisPoint_ = glm::vec3(0.f);
}

ZHGameObject ZTransformGizmo::CreateAxisArrow(const glm::vec3& axis)
{
    // TODO: Calling Rebuild on this model means it is getting build twice! Try passing data via create calls directly, maybe by template forwarding arguments.
    static ZHModel cylinder = ZServices::ModelManager()->Create(ZModelType::Cylinder);
    ZServices::ModelManager()->Dereference<ZCylinder>(cylinder)->Rebuild(1.0f, 1.0f, 1.0f, glm::vec2(8.f));

    // TODO: Calling Rebuild on this model means it is getting build twice! Try passing data via create calls directly, maybe by template forwarding arguments.
    static ZHModel cone = ZServices::ModelManager()->Create(ZModelType::Cylinder);
    ZServices::ModelManager()->Dereference<ZCylinder>(cone)->Rebuild(0.0f, 0.5f, 1.0f, glm::vec2(8.f));

    ZMaterialProperties materialProps;
    materialProps.albedo = glm::vec4(glm::abs(axis), 1.f);
    auto arrowMaterial = ZServices::MaterialManager()->Create(materialProps, ZServices::ShaderManager()->Create("/Shaders/Vertex/gizmo.vert", "/Shaders/Pixel/gizmo.frag"));

    auto axisArrow = ZServices::GameObjectManager()->Create(ZGameObjectType::Custom);

    auto arrowBase = ZServices::GameObjectManager()->Create(ZGameObjectType::Custom);
    ZServices::GameObjectManager()->SetPosition(arrowBase, glm::vec3(0.f, 0.f, 2.5f));
    ZServices::GameObjectManager()->SetScale(arrowBase, glm::vec3(0.03f, 0.03f, 5.f));
    ZServices::GameObjectManager()->SetRenderOrder(arrowBase, ZRenderLayer::UI);

    auto graphicsComp = ZServices::ComponentManager()->CreateIn(ZComponentType::Graphics, arrowBase);
    auto graphicsCompObj = ZServices::ComponentManager()->Dereference<ZGraphicsComponent>(graphicsComp);
    graphicsCompObj->Initialize(cylinder);
    graphicsCompObj->isShadowCaster = false;
    graphicsCompObj->hasDepthInfo = false;
    graphicsCompObj->AddMaterial(arrowMaterial);

    auto arrowTop = ZServices::GameObjectManager()->Create(ZGameObjectType::Custom);
    ZServices::GameObjectManager()->SetPosition(arrowTop, glm::vec3(0.f, 0.f, 5.f));
    ZServices::GameObjectManager()->SetScale(arrowTop, glm::vec3(0.5f, 0.5f, 0.5f));
    ZServices::GameObjectManager()->SetRenderOrder(arrowTop, ZRenderLayer::UI);

    graphicsComp = ZServices::ComponentManager()->CreateIn(ZComponentType::Graphics, arrowTop);
    graphicsCompObj = ZServices::ComponentManager()->Dereference<ZGraphicsComponent>(graphicsComp);
    graphicsCompObj->Initialize(cone);
    graphicsCompObj->isShadowCaster = false;
    graphicsCompObj->hasDepthInfo = false;
    graphicsCompObj->AddMaterial(arrowMaterial);

    ZServices::GameObjectManager()->AddChild(axisArrow, arrowBase);
    ZServices::GameObjectManager()->AddChild(axisArrow, arrowTop);

    ZServices::GameObjectManager()->SetOrientation(axisArrow, axis * PI * 0.5f);

    return axisArrow;
}
