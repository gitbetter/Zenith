/*

 ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
 /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
 \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

 ZMaterial.hpp

 Created by Adrian Sanchez on 01/02/2019.
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
#include "ZOFTree.hpp"

// Forward Declarations
class ZTexture;
class ZTextureReadyEvent;
class ZShaderReadyEvent;
class ZRenderStateGroup;
class ZUniformBuffer;

// Class and Data Structure Definitions

struct ZMaterialProperties
{
    float alpha{ 1.f };
    float tiling{ 1.f };
    bool isPBR{ false };
    bool hasDisplacement{ false };
    union
    {
        Material look;
        PBRMaterial realisticLook;
    };

    ZMaterialProperties()
    {
        look.albedo = glm::vec4(1.f, 1.f, 1.f, 1.f); look.emission = 0.f;
        look.diffuse = 0.8f; look.ambient = 0.2f;
        look.specular = 0.5f; look.shininess = 48.f;
    }
};

class ZMaterial : public std::enable_shared_from_this<ZMaterial>
{

public:

    ZMaterial();
    ZMaterial(const ZMaterialProperties& materialProperties, const std::shared_ptr<ZShader>& shader = nullptr);
    ZMaterial(const std::vector<std::shared_ptr<ZTexture>>& textures, const std::shared_ptr<ZShader>& shader = nullptr);

    void Initialize();

    const std::string& ID() const { return id_; }
    const ZMaterialProperties& Properties() const { return properties_; }
    const ZTextureMap& Textures() const { return textures_; }
    float Alpha(float alpha) const { return properties_.alpha; }
    bool IsPBR() const { return properties_.isPBR; }
    bool IsTextured() const { return !textures_.empty(); }
    bool HasDisplacement() const { return properties_.hasDisplacement; }
    const std::shared_ptr<ZRenderStateGroup>& RenderState() const { return renderState_; }

    void SetShader(const std::shared_ptr<ZShader>& shader);
    void SetPBR(bool pbr = true) { properties_.isPBR = pbr; }
    void SetProperties(const ZMaterialProperties& properties) { properties_ = properties; }
    void SetAlpha(float alpha) { properties_.alpha = alpha; }
    void SetProperty(const std::string& property, float value);
    void SetProperty(const std::string& property, const glm::vec4& value);
    void SetProperty(const std::string& property, bool value);

    void AddTexture(const std::string& slot, const std::shared_ptr<ZTexture>& texture);
    void AddTexture(const std::string& slot, const std::string& textureId) { pendingTextures_[slot] = textureId; }

    static std::shared_ptr<ZMaterial> Default();
    static std::shared_ptr<ZMaterial> CreateDefault();
    static void Create(std::shared_ptr<ZOFTree> data, ZMaterialMap& outTextureMap, const ZTextureMap& textureCache);
    static void CreateAsync(std::shared_ptr<ZOFTree> data, ZMaterialIDMap& outPendingTextures, ZMaterialMap& outMaterials);

    static std::shared_ptr<ZMaterial> Create(const ZMaterialProperties& materialProperties, const std::shared_ptr<ZShader>& shader = nullptr);
    static std::shared_ptr<ZMaterial> Create(const std::vector<std::shared_ptr<ZTexture>>& textures, const std::shared_ptr<ZShader>& shader = nullptr);

private:

    std::string id_;
    ZMaterialProperties properties_;
    ZTextureMap textures_;
    std::unordered_map<std::string, std::string> pendingTextures_;

    std::string shaderId_;
    std::shared_ptr<ZShader> shaderObject_ = nullptr;

    std::shared_ptr<ZRenderStateGroup> renderState_;
    std::shared_ptr<ZUniformBuffer> uniformBuffer_;

    static ZIDSequence idGenerator_;

    void UpdateUniformMaterial();

    void HandleTextureReady(const std::shared_ptr<ZTextureReadyEvent>& event);
    void HandleShaderReady(const std::shared_ptr<ZShaderReadyEvent>& event);

};
