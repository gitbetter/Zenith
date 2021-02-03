/*
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

   ZSceneRoot.cpp

   Created by Adrian Sanchez on 19/04/19.
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

#include "ZSceneRoot.hpp"
#include "ZServices.hpp"
#include "ZPhysicsUniverse.hpp"
#include "ZScene.hpp"

ZSceneRoot::ZSceneRoot(const std::string& name) : ZGameObject(name)
{
    std::shared_ptr<ZGameObject> staticGroup = std::make_shared<ZGameObject>("StaticGroup");
    staticGroup->SetRenderOrder(ZRenderOrder::Static);
    children_.push_back(staticGroup);

    std::shared_ptr<ZGameObject> dynamicGroup = std::make_shared<ZGameObject>("DynamicGroup");
    dynamicGroup->SetRenderOrder(ZRenderOrder::Dynamic);
    children_.push_back(dynamicGroup);

    std::shared_ptr<ZGameObject> skyGroup = std::make_shared<ZGameObject>("SkyGroup");
    skyGroup->SetRenderOrder(ZRenderOrder::Sky);
    children_.push_back(skyGroup);

    std::shared_ptr<ZGameObject> invisibleGroup = std::make_shared<ZGameObject>("InvisibleGroup");
    invisibleGroup->SetRenderOrder(ZRenderOrder::Invisible);
    children_.push_back(invisibleGroup);

    std::shared_ptr<ZGameObject> uiGroup = std::make_shared<ZGameObject>("UIGroup");
    skyGroup->SetRenderOrder(ZRenderOrder::Sky);
    children_.push_back(uiGroup);
}

void ZSceneRoot::AddChild(std::shared_ptr<ZGameObject> gameObject)
{
    ZRenderOrder order = gameObject->RenderOrder();
    if (order >= children_.size() || !children_[order])
    {
        LOG("The child being added has a non-extant render order", ZSeverity::Warning);
        return;
    }

    children_[order]->AddChild(gameObject);

    if (std::find(publicChildren_.begin(), publicChildren_.end(), gameObject) == publicChildren_.end())
    {
        publicChildren_.push_back(gameObject);
    }

    gameObject->parent_ = shared_from_this();
}

void ZSceneRoot::RemoveChild(std::shared_ptr<ZGameObject> gameObject, bool recurse)
{
    ZGameObject::RemoveChild(gameObject);
    for (ZGameObjectList::iterator it = children_.begin(), end = children_.end(); it != end; it++)
    {
        (*it)->RemoveChild(gameObject, recurse);
    }

    ZGameObjectList::iterator it = std::find(publicChildren_.begin(), publicChildren_.end(), gameObject);
    if (it != publicChildren_.end())
        publicChildren_.erase(it);
}

void ZSceneRoot::Render(double deltaTime, const std::shared_ptr<ZShader>& shader, ZRenderOp renderOp)
{
    RenderChildren(deltaTime, shader, renderOp);
}

void ZSceneRoot::RenderChildren(double deltaTime, const std::shared_ptr<ZShader>& shader, ZRenderOp renderOp)
{
    auto scene = Scene();
    if (!scene) return;

    if (scene->GameConfig().graphics.drawGrid)
        ZServices::Graphics()->DebugDrawGrid(scene, glm::vec4(0.75f, 0.75f, 0.75f, 1.f));

    for (int pass = ZRenderOrder::First; pass < ZRenderOrder::Last; pass++)
    {
        switch (pass)
        {
        case ZRenderOrder::Static:
        case ZRenderOrder::Dynamic:
        case ZRenderOrder::Sky:
        case ZRenderOrder::UI:
            children_[pass]->RenderChildren(deltaTime, shader, renderOp);
            break;
        default: break;
        }
    }

    if (scene->GameConfig().graphics.drawPhysicsDebug)
        scene->PhysicsUniverse()->DebugDraw(scene);
}

DEFINE_OBJECT_CREATORS(ZSceneRoot)