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
#include "ZMesh.hpp"
#include "ZRenderTask.hpp"
#include "ZRenderPass.hpp"
#include "ZCamera.hpp"

ZSceneRoot::ZSceneRoot(const std::string& name) : ZGameObject(name)
{ }

void ZSceneRoot::Initialize()
{
    ZRenderStateGroupWriter writer;
    writer.Begin();
    writer.SetRenderLayer(ZRenderLayer::UI);
    renderState_ = writer.End();

    ZGameObject::Initialize();
}

std::shared_ptr<ZMesh2D> ZSceneRoot::ScreenTri()
{
    static std::shared_ptr<ZMesh2D> screenTri = ZMesh2D::NewScreenTriangle();
    return screenTri;
}

void ZSceneRoot::Prepare(double deltaTime)
{
    auto scene = Scene();
    if (!scene) return;

    auto cam = scene->ActiveCamera();
    if (!cam) return;

    auto mesh = ScreenTri();

    auto meshState = mesh->RenderState();
    auto cameraState = cam->RenderState();

    ZDrawCall drawCall = ZDrawCall::Create(ZMeshDrawStyle::Triangle);
    auto renderTask = ZRenderTask::Compile(drawCall,
        { cameraState, meshState, renderState_ },
        ZRenderPass::Post()
    );
    renderTask->Submit({ ZRenderPass::Post() });

    PrepareChildren(deltaTime);
}

void ZSceneRoot::PrepareChildren(double deltaTime)
{
    auto scene = Scene();
    if (!scene) return;

    if (scene->GameConfig().graphics.drawGrid)
        ZServices::Graphics()->DebugDrawGrid(scene, glm::vec4(0.75f, 0.75f, 0.75f, 1.f));

    ZGameObject::PrepareChildren(deltaTime);

    if (scene->GameConfig().graphics.drawPhysicsDebug)
        scene->PhysicsUniverse()->DebugDraw(scene);
}

DEFINE_OBJECT_CREATORS(ZSceneRoot)