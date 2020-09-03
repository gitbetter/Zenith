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
#include "ZEngine.hpp"

// Forward Declarations
class ZGameObject;
class ZWindow;
class ZShader;
class ZGraphicsStrategy;
class ZGraphicsDebug;
class ZLight;
class ZModel;
class ZMesh2D;
class ZScene;

// Class and Data Structure Definitions
typedef std::map<std::string, ZTexture> ZTextureMap;

class ZGraphics
{

private:

    std::map<std::string, std::string> pendingTextures_;
    std::map<std::shared_ptr<ZShader>, std::string> pendingShaders_;
    std::map<std::shared_ptr<ZModel>, std::string> pendingModels_;

    void InitializeBuffers();
    void InitializeShaders();

public:

    ZGraphics() {}
    ~ZGraphics() {}

    void Initialize();
    void Load(std::shared_ptr<ZOFTree> root);
    void LoadAsync(std::shared_ptr<ZOFTree> root);

    void SetupShadowDepthPass(std::shared_ptr<ZLight> light);
    void SetupDepthPass();
    void SetupColorPass();
    void PostProcessing(ZScene* scene);
    void FinishRenderPass();

    ZGraphicsStrategy* Strategy() { return graphicsStrategy_.get(); }
    ZGraphicsDebug* DebugDrawer() { return debugDrawer_.get(); }
    glm::mat4 LightSpaceMatrix() { return currentLightSpaceMatrix_; }
    ZTexture ShadowBuffer() { return shadowBuffer_; }
    ZTexture DepthBuffer() { return depthBuffer_; }
    ZTexture ColorBuffer() { return colorBuffer_; }
    ZTexture SceneBuffer() { return postBuffer_; }
    std::shared_ptr<ZShader> ShadowShader() { return shadowShader_; }
    std::shared_ptr<ZShader> DepthShader() { return depthShader_; }
    ZTextureMap& Textures() { return loadedTextures_; }
    ZShaderMap& Shaders() { return loadedShaders_; }
    ZModelMap& Models() { return loadedModels_; }

    void AddShader(std::string id, std::shared_ptr<ZShader> shader);
    void AddTexture(std::string id, ZTexture texture);
    void AddModel(std::string id, std::shared_ptr<ZModel> model);

    static void ComputeTangentBitangent(ZVertex3D& v1, ZVertex3D& v2, ZVertex3D& v3);

    void CleanUp();

    void HandleShaderReady(const std::shared_ptr<ZEvent>& event);
    void HandleTextureReady(const std::shared_ptr<ZEvent>& event);
    void HandleModelReady(const std::shared_ptr<ZEvent>& event);

protected:

    std::shared_ptr<ZGraphicsStrategy> graphicsStrategy_ = nullptr;
    std::shared_ptr<ZGraphicsDebug> debugDrawer_ = nullptr;
    std::shared_ptr<ZShader> shadowShader_ = nullptr;
    std::shared_ptr<ZShader> depthShader_ = nullptr;
    std::shared_ptr<ZShader> postShader_ = nullptr;
    std::shared_ptr<ZMesh2D> renderQuad_ = nullptr;
    ZBufferData shadowFrameBuffer_, depthFrameBuffer_, colorFrameBufferMultisampled_;
    ZBufferData colorFrameBuffer_, postFrameBuffer_;
    ZTexture shadowBuffer_, depthBuffer_, colorBufferMultisampled_;
    ZTexture colorBuffer_, postBuffer_;
    glm::mat4 currentLightSpaceMatrix_;
    ZShaderMap loadedShaders_;
    ZTextureMap loadedTextures_;
    ZModelMap loadedModels_;
};
