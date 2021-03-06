/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZGLGraphics.cpp

    Created by Adrian Sanchez on 27/01/2019.
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

#include "ZServices.hpp"
#include "ZVertexBuffer.hpp"
#include "ZScene.hpp"
#include "ZFrustum.hpp"
#include "ZAssetStore.hpp"
#include "ZShader.hpp"
#include "ZCamera.hpp"
#include "ZRenderTask.hpp"
#include "ZRenderPass.hpp"
#include "ZRenderStateExecutor.hpp"

std::shared_ptr<ZRenderStateExecutor> ZGraphics::Executor()
{
    if (!executor_) {
        executor_ = ZRenderStateExecutor::Create();
    }
    return executor_;
}

void ZGraphics::DebugDraw(const std::shared_ptr<ZScene>& scene, const ZFrustum& frustum, const glm::vec4& color)
{
    std::vector<std::pair<glm::vec3, glm::vec3>> linePoints = {
        std::make_pair<glm::vec3, glm::vec3>(glm::vec3(frustum.corners[0]), glm::vec3(frustum.corners[1])),
        std::make_pair<glm::vec3, glm::vec3>(glm::vec3(frustum.corners[1]), glm::vec3(frustum.corners[2])),
        std::make_pair<glm::vec3, glm::vec3>(glm::vec3(frustum.corners[2]), glm::vec3(frustum.corners[3])),
        std::make_pair<glm::vec3, glm::vec3>(glm::vec3(frustum.corners[3]), glm::vec3(frustum.corners[0])),
        std::make_pair<glm::vec3, glm::vec3>(glm::vec3(frustum.corners[4]), glm::vec3(frustum.corners[5])),
        std::make_pair<glm::vec3, glm::vec3>(glm::vec3(frustum.corners[5]), glm::vec3(frustum.corners[6])),
        std::make_pair<glm::vec3, glm::vec3>(glm::vec3(frustum.corners[6]), glm::vec3(frustum.corners[7])),
        std::make_pair<glm::vec3, glm::vec3>(glm::vec3(frustum.corners[7]), glm::vec3(frustum.corners[4])),
        std::make_pair<glm::vec3, glm::vec3>(glm::vec3(frustum.corners[0]), glm::vec3(frustum.corners[4])),
        std::make_pair<glm::vec3, glm::vec3>(glm::vec3(frustum.corners[1]), glm::vec3(frustum.corners[5])),
        std::make_pair<glm::vec3, glm::vec3>(glm::vec3(frustum.corners[2]), glm::vec3(frustum.corners[6])),
        std::make_pair<glm::vec3, glm::vec3>(glm::vec3(frustum.corners[3]), glm::vec3(frustum.corners[7]))
    };

    for (const auto& points : linePoints)
    {
        DebugDrawLine(scene, points.first, points.second, color);
    }
}

void ZGraphics::DebugDraw(const std::shared_ptr<ZScene>& scene, const ZAABBox& aabb, const glm::vec4& color)
{
    auto cornerTLB = glm::vec3(aabb.minimum.x, aabb.maximum.y, aabb.minimum.z);
    auto cornerTRB = glm::vec3(aabb.maximum.x, aabb.maximum.y, aabb.minimum.z);
    auto cornerBRB = glm::vec3(aabb.maximum.x, aabb.minimum.y, aabb.minimum.z);
    auto cornerBLB = glm::vec3(aabb.minimum.x, aabb.minimum.y, aabb.minimum.z);
    auto cornerTLF = glm::vec3(aabb.minimum.x, aabb.maximum.y, aabb.maximum.z);
    auto cornerTRF = glm::vec3(aabb.maximum.x, aabb.maximum.y, aabb.maximum.z);
    auto cornerBRF = glm::vec3(aabb.maximum.x, aabb.minimum.y, aabb.maximum.z);
    auto cornerBLF = glm::vec3(aabb.minimum.x, aabb.minimum.y, aabb.maximum.z);
    std::vector<std::pair<glm::vec3, glm::vec3>> linePoints = {
        { cornerTLB, cornerTRB },
        { cornerTRB, cornerBRB },
        { cornerBRB, cornerBLB },
        { cornerBLB, cornerTLB },
        { cornerTLF, cornerTRF },
        { cornerTRF, cornerBRF },
        { cornerBRF, cornerBLF },
        { cornerBLF, cornerTLF },
        { cornerTLB, cornerTLF },
        { cornerTRB, cornerTRF },
        { cornerBRB, cornerBRF },
        { cornerBLB, cornerBLF }
    };

    for (const auto& points : linePoints)
    {
        DebugDrawLine(scene, points.first, points.second, color);
    }
}

void ZGraphics::DebugDrawGrid(const std::shared_ptr<ZScene>& scene, const glm::vec4& color)
{
    std::vector<std::pair<glm::vec3, glm::vec3>> linePoints;
    int numGridLines = 400, gridCellSize = 1;
    for (int i = -numGridLines / 2, j = numGridLines / 2; i < j; i += (gridCellSize * 2))
    {
        linePoints.push_back(std::make_pair<glm::vec3, glm::vec3>(glm::vec3(-(float)numGridLines / 2.f, 0.f, (float)i), glm::vec3((float)numGridLines / 2.f, 0.f, (float)i)));
        linePoints.push_back(std::make_pair<glm::vec3, glm::vec3>(glm::vec3((float)i, 0.f, -(float)numGridLines / 2.f), glm::vec3((float)i, 0.f, (float)numGridLines / 2.f)));
    }

    for (const auto& points : linePoints)
    {
        DebugDrawLine(scene, points.first, points.second, color);
    }
}

// TODO: Pass pointer to scene to this method
void ZGraphics::DebugDrawLine(const std::shared_ptr<ZScene>& scene, const glm::vec3& from, const glm::vec3& to, const glm::vec4& color)
{
    if (!scene) return;

    auto cam = scene->ActiveCamera();
    if (!cam) return;

    ZPR_SESSION_COLLECT_VERTICES(2);

    ZVertex3DDataOptions options;
    options.vertices = ZVertex3DList{
      ZVertex3D(from), ZVertex3D(to)
    };

    auto bufferData = ZVertexBuffer::Create(options);
    auto colorUniforms = ZUniformBuffer::Create(ZUniformBufferType::UI, sizeof(ZUIUniforms));
    colorUniforms->Update(offsetof(ZUIUniforms, color), sizeof(color), &color);

    ZRenderStateGroupWriter writer;
    writer.Begin();
    writer.BindVertexBuffer(bufferData);
    writer.SetShader(ZServices::AssetStore()->DebugShader());
    writer.BindUniformBuffer(colorUniforms);
    auto lineState = writer.End();

    ZDrawCall drawCall = ZDrawCall::Create(ZMeshDrawStyle::Line);
    auto renderTask = ZRenderTask::Compile(drawCall,
        { cam->RenderState(), lineState },
        ZRenderPass::Color()
    );
    renderTask->Submit({ ZRenderPass::Color() });

    // TODO: Delete buffer data once rendering is done, maybe within a Debug renderpass?
    // Might also want to think about a special debug renderer implementation instead
}