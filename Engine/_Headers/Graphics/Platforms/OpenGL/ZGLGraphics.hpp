/*

 ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
 /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
 \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

 ZGLGraphics.hpp

 Created by Adrian Sanchez on 27/01/2021.
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

#define GLM_ENABLE_EXPERIMENTAL

// Includes
#include "ZGraphics.hpp"

// Forward Declarations

// Class and Data Structure Definitions
class ZGLGraphics : public ZGraphics
{

private:

    std::map<ZMeshDrawStyle, unsigned int> drawingStylesMap_;

    static void GLFWErrorCallback(int id, const char* description);

public:

    ZGLGraphics(const ZGraphicsOptions& options = ZGraphicsOptions()) : ZGraphics(options) {}
    ~ZGLGraphics() {}

    // Platform Graphics
    void Initialize() override;
    void ClearViewport(const glm::vec4& clearColor = glm::vec4(0.f, 0.f, 0.f, 1.f)) override;
    void UpdateViewport(const glm::vec2& size, const glm::vec2& position = glm::vec2(0.f)) override;
    void EnableStencilTesting() override;
    void EnableDepthTesting() override;
    void DisableDepthTesting() override;
    void EnableStencilBuffer() override;
    void DisableStencilBuffer() override;
    void EnableAlphaBlending() override;
    void DisableAlphaBlending() override;
    void EnableFaceCulling() override;
    void DisableFaceCulling() override;
    void EnableSeamlessCubemap() override;
    void DisableSeamlessCubemap() override;
    void EnableMSAA() override;
    void DisableMSAA() override;
    void CullFrontFaces() override;
    void CullBackFaces() override;
    void ClearDepth() override;
    void Draw(const std::shared_ptr<ZBuffer>& bufferData, const ZVertex3DDataOptions& vertexData, ZMeshDrawStyle drawStyle = ZMeshDrawStyle::Triangle) override;
    void Draw(const std::shared_ptr<ZBuffer>& bufferData, const ZVertex2DDataOptions& vertexData, ZMeshDrawStyle drawStyle = ZMeshDrawStyle::TriangleStrip) override;

};
