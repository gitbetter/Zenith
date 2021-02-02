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
#include "ZWindowResizeEvent.hpp"
#include "ZTextureReadyEvent.hpp"
#include "ZShaderReadyEvent.hpp"
#include "ZModelReadyEvent.hpp"
#include "ZFontReadyEvent.hpp"
#include "ZGameObject.hpp"
#include "ZShader.hpp"
#include "ZModel.hpp"
#include "ZMesh2D.hpp"
#include "ZScene.hpp"
#include "ZLight.hpp"
#include "ZFont.hpp"
#include "ZAssetStore.hpp"

void ZGraphics::DrawText(const std::shared_ptr<ZBuffer>& bufferData, const glm::vec2& position, const std::string& text, const std::shared_ptr<ZFont>& font, float fontScale, float lineSpacing, float maxWrap)
{
    auto pos = position;
    float x = pos.x, y = pos.y;

    ZVertex2DDataOptions options;
    options.vertices.reserve(text.size() * 6);
    for (auto c = text.begin(); c != text.end(); c++)
    {
        ZCharacter character = font->Atlas().characterInfo[*c];

        float one_over_font_size = 1.f / font->Size();

        float atlasH = font->Atlas().height * 0.5f * fontScale * one_over_font_size;
        float w = character.bitmapSize.x * fontScale * one_over_font_size;
        float h = character.bitmapSize.y * fontScale * one_over_font_size;
        float xpos = x + character.bitmapPos.x * fontScale * one_over_font_size;
        float ypos = (y + atlasH) - character.bitmapPos.y * fontScale * one_over_font_size;

        x += character.advance.x * fontScale * one_over_font_size;
        y += character.advance.y * fontScale * one_over_font_size;

        if (maxWrap > 0.f)
        {
            auto maxX = maxWrap - w * 2.f;
            if (x > maxX)
            {
                y += std::floor(x / maxX) * (atlasH + lineSpacing);
                x = pos.x;
            }
        }

        if (w == 0 || h == 0) continue;

        options.vertices.push_back(ZVertex2D(glm::vec2(xpos, ypos), glm::vec2(character.xOffset, 0)));
        options.vertices.push_back(ZVertex2D(glm::vec2(xpos + w, ypos), glm::vec2(character.xOffset + character.bitmapSize.x / font->Atlas().width, 0)));
        options.vertices.push_back(ZVertex2D(glm::vec2(xpos, ypos + h), glm::vec2(character.xOffset, character.bitmapSize.y / font->Atlas().height)));
        options.vertices.push_back(ZVertex2D(glm::vec2(xpos + w, ypos), glm::vec2(character.xOffset + character.bitmapSize.x / font->Atlas().width, 0)));
        options.vertices.push_back(ZVertex2D(glm::vec2(xpos, ypos + h), glm::vec2(character.xOffset, character.bitmapSize.y / font->Atlas().height)));
        options.vertices.push_back(ZVertex2D(glm::vec2(xpos + w, ypos + h), glm::vec2(character.xOffset + character.bitmapSize.x / font->Atlas().width, character.bitmapSize.y / font->Atlas().height)));
    }

    bufferData->Update(options);
    Draw(bufferData, options, ZMeshDrawStyle::Triangle);
}

// TODO: Pass pointer to scene to this method
void ZGraphics::DebugDraw(const std::shared_ptr<ZScene>& scene, ZFrustum& frustum, const glm::vec4& color)
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
        DebugDrawLine(scene, points.first, points.second, color);
    }
}

// TODO: Pass pointer to scene to this method
void ZGraphics::DebugDrawGrid(const std::shared_ptr<ZScene>& scene, const glm::vec4& color)
{
    std::vector<std::pair<glm::vec3, glm::vec3>> linePoints;
    int numGridLines = 200, gridCellSize = 1;
    for (int i = -numGridLines / 2, j = numGridLines / 2; i < j; i += (gridCellSize * 2))
    {
        linePoints.push_back(std::make_pair<glm::vec3, glm::vec3>(glm::vec3(-(float)numGridLines / 2.f, 0.f, (float)i), glm::vec3((float)numGridLines / 2.f, 0.f, (float)i)));
        linePoints.push_back(std::make_pair<glm::vec3, glm::vec3>(glm::vec3((float)i, 0.f, -(float)numGridLines / 2.f), glm::vec3((float)i, 0.f, (float)numGridLines / 2.f)));
    }

    for (std::pair<glm::vec3, glm::vec3> points : linePoints)
    {
        DebugDrawLine(scene, points.first, points.second, color);
    }
}

// TODO: Pass pointer to scene to this method
void ZGraphics::DebugDrawLine(const std::shared_ptr<ZScene>& scene, const glm::vec3& from, const glm::vec3& to, const glm::vec4& color)
{
    if (!scene) return;

    auto VPMatrix = scene->ViewProjection();
    auto debugShader = scene->AssetStore()->DebugShader();

    debugShader->Activate();
    debugShader->SetMat4("ViewProjection", VPMatrix);
    debugShader->SetVec4("color", color);

    ZVertex3DDataOptions options;
    options.vertices = std::vector<ZVertex3D>{
      ZVertex3D(from), ZVertex3D(to)
    };

    ZBuffer::ptr bufferData = ZBuffer::Create(options);
    Draw(bufferData, options, ZMeshDrawStyle::Line);
    bufferData->Delete();
}

void ZGraphics::ComputeTangentBitangent(ZVertex3D& v1, ZVertex3D& v2, ZVertex3D& v3)
{
    glm::vec3 deltaPos1 = v2.position - v1.position;
    glm::vec3 deltaPos2 = v3.position - v1.position;
    glm::vec2 deltaUV1 = v2.uv - v1.uv;
    glm::vec2 deltaUV2 = v3.uv - v1.uv;

    float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
    glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
    glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

    v1.tangent = tangent; v2.tangent = tangent; v3.tangent = tangent;
    v1.bitangent = bitangent; v2.bitangent = bitangent; v3.bitangent = bitangent;
}