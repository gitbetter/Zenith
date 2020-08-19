/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZGraphicsDebug.cpp

    Created by Adrian Sanchez on 24/05/2019.
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

#include "ZGraphicsDebug.hpp"
#include "ZShader.hpp"
#include "ZGame.hpp"

void ZGraphicsDebug::Initialize()
{
    shader_ = std::unique_ptr<ZShader>(new ZShader(ENGINE_ASSETS_PATH + "/Shaders/Vertex/debug.vert", ENGINE_ASSETS_PATH + "/Shaders/Pixel/debug.frag"));
    shader_->Initialize();
}

void ZGraphicsDebug::Draw(ZFrustum& frustum, const glm::vec4& color)
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

    for (std::pair<glm::vec3, glm::vec3> points : linePoints)
    {
        DrawLine(points.first, points.second, color);
    }
}

void ZGraphicsDebug::DrawGrid(const glm::vec4& color)
{
    std::vector<std::pair<glm::vec3, glm::vec3>> linePoints;
    int numGridLines = 200, gridCellSize = 1;
    for (int i = -numGridLines / 2, j = numGridLines / 2; i < j; i += (gridCellSize * 2))
    {
        linePoints.push_back(std::make_pair<glm::vec3, glm::vec3>(glm::vec3(-(float) numGridLines / 2.f, 0.f, (float) i), glm::vec3((float) numGridLines / 2.f, 0.f, (float) i)));
        linePoints.push_back(std::make_pair<glm::vec3, glm::vec3>(glm::vec3((float) i, 0.f, -(float) numGridLines / 2.f), glm::vec3((float) i, 0.f, (float) numGridLines / 2.f)));
    }

    for (std::pair<glm::vec3, glm::vec3> points : linePoints)
    {
        DrawLine(points.first, points.second, color);
    }
}

void ZGraphicsDebug::DrawLine(const glm::vec3& from, const glm::vec3& to, const glm::vec4& color)
{
    if (!zenith::Game()->ActiveScene()) return;

    glm::mat4 VPMatrix = zenith::Game()->ActiveScene()->ViewProjection();

    shader_->Activate();

    shader_->SetMat4("ViewProjection", VPMatrix);
    shader_->SetVec4("color", color);

    std::vector<ZVertex3D> vertices({
      ZVertex3D(from), ZVertex3D(to)
        });

    ZBufferData bufferData = zenith::Graphics()->Strategy()->LoadVertexData(vertices);
    zenith::Graphics()->Strategy()->DrawLines(bufferData, vertices);
    zenith::Graphics()->Strategy()->DeleteBufferData(bufferData);
}
