/*

 ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
 /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
 \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

 ZGraphics.hpp

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

#pragma once

// Includes
#include "ZCommon.hpp"

// Forward Declarations
class ZScene;
class ZFrustum;
class ZBuffer;
class ZFont;

// Class and Data Structure Definitions
class ZGraphics
{

public:

    ZGraphics(const ZGraphicsOptions& options) : options_(options) {}
    ~ZGraphics() {}

    void UsePBRPipeline(bool pbr = true) { options_.hasPBR = pbr; }
    void UseMotionBlur(bool blur = false) { options_.hasMotionBlur = blur; }
    bool HasPBR() const { return options_.hasPBR; }
    bool HasMotionBlur() const { return options_.hasMotionBlur; }

    // Platform Graphics
    virtual void Initialize() = 0;
    virtual void ClearViewport() = 0; 
    virtual void UpdateViewport(const glm::vec2& size, const glm::vec2& position = glm::vec2(0.f)) = 0;
    virtual void EnableStencilTesting() = 0;
    virtual void EnableDepthTesting() = 0;
    virtual void DisableDepthTesting() = 0;
    virtual void EnableStencilBuffer() = 0;
    virtual void DisableStencilBuffer() = 0;
    virtual void EnableAlphaBlending() = 0;
    virtual void DisableAlphaBlending() = 0;
    virtual void EnableFaceCulling() = 0;
    virtual void DisableFaceCulling() = 0;
    virtual void EnableSeamlessCubemap() = 0;
    virtual void DisableSeamlessCubemap() = 0;
    virtual void EnableMSAA() = 0;
    virtual void DisableMSAA() = 0;
    virtual void CullFrontFaces() = 0;
    virtual void CullBackFaces() = 0;
    virtual void ClearDepth() = 0;
    virtual void Draw(const std::shared_ptr<ZBuffer>& bufferData, const ZVertex3DDataOptions& vertexData, ZMeshDrawStyle drawStyle = ZMeshDrawStyle::Triangle) = 0;
    virtual void Draw(const std::shared_ptr<ZBuffer>& bufferData, const ZVertex2DDataOptions& vertexData, ZMeshDrawStyle drawStyle = ZMeshDrawStyle::TriangleStrip) = 0;

    void DrawText(const std::shared_ptr<ZBuffer>& bufferData, const glm::vec2& position, const std::string& text, const std::shared_ptr<ZFont>& font, float fontScale = 1.f, float lineSpacing = 1.f, float maxWrap = 0.f);
    void DebugDraw(const std::shared_ptr<ZScene>& scene, ZFrustum& frustum, const glm::vec4& color);
    void DebugDrawGrid(const std::shared_ptr<ZScene>& scene, const glm::vec4& color);
    void DebugDrawLine(const std::shared_ptr<ZScene>& scene, const glm::vec3& from, const glm::vec3& to, const glm::vec4& color);

    // Vertex3D
    static void ComputeTangentBitangent(ZVertex3D& v1, ZVertex3D& v2, ZVertex3D& v3);

protected:

    ZGraphicsOptions options_;

};
