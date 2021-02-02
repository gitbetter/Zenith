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

// Forward Declarations
class ZTexture;
class ZTextureReadyEvent;

// Class and Data Structure Definitions
class ZMaterial : public std::enable_shared_from_this<ZMaterial>
{

public:

    ZMaterial(int index = 0);
    ZMaterial(const ZMaterialProperties& materialProperties) : ZMaterial(0) { properties_ = materialProperties; }
    ZMaterial(const std::vector<std::shared_ptr<ZTexture>>& textures);

    void Initialize();

    const std::string& ID() const { return id_; }
    const ZMaterialProperties& Properties() const { return properties_; }
    const ZTextureMap& Textures() const { return textures_; }
    float Alpha(float alpha) const { return properties_.alpha; }
    int Index() const { return index_; }
    bool IsPBR() const { return properties_.isPBR; }
    bool IsTextured() const { return !textures_.empty(); }
    bool HasDisplacement() const { return properties_.hasDisplacement; }

    void SetPBR(bool pbr = true) { properties_.isPBR = pbr; }
    void SetProperties(const ZMaterialProperties& properties) { properties_ = properties; }
    void SetAlpha(float alpha) { properties_.alpha = alpha; }
    void SetProperty(const std::string& property, float value);
    void SetProperty(const std::string& property, const glm::vec4& value);
    void SetProperty(const std::string& property, bool value);

    void AddTexture(const std::string& slot, const std::shared_ptr<ZTexture>& texture) { textures_[slot] = texture; }
    void AddTexture(const std::string& slot, const std::string& textureId) { pendingTextures_[slot] = textureId; }

    static std::shared_ptr<ZMaterial> Default();
    static std::shared_ptr<ZMaterial> CreateDefault();
    static void Create(std::shared_ptr<ZOFTree> data, ZMaterialMap& outTextureMap, const ZTextureMap& textureCache);
    static void CreateAsync(std::shared_ptr<ZOFTree> data, ZMaterialIDMap& outPendingTextures);
    static std::shared_ptr<ZMaterial> Create(const ZMaterialProperties& materialProperties);
    static std::shared_ptr<ZMaterial> Create(const std::vector<std::shared_ptr<ZTexture>>& textures);

private:

    std::string id_;
    ZMaterialProperties properties_;
    ZTextureMap textures_;
    std::unordered_map<std::string, std::string> pendingTextures_;
    int index_;
    bool isPBR_ = false;
    bool hasDisplacement_ = false;

    static ZIDSequence idGenerator_;

    void HandleTextureReady(const std::shared_ptr<ZTextureReadyEvent>& event);

};
