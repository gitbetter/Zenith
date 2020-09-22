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
#include "ZEngine.hpp"

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
class ZMaterial
{

private:

    ZMaterialProperties properties_;
    std::vector<ZTexture> textures_;
    std::map<std::string, std::string> pendingTextures_;
    std::string meshId_;
    int index_;
    bool isPBR_ = false, hasDisplacement_ = false;

    void SetMaterialProperty(const std::string& property, float value, ZMaterialProperties& materialProperties);
    void SetMaterialProperty(const std::string& property, const glm::vec4& value, ZMaterialProperties& materialProperties);

public:

    ZMaterial(int index = 0) { index_ = index; }
    ZMaterial(const ZMaterialProperties& materialProperties) : ZMaterial(0) { properties_ = materialProperties; }
    ZMaterial(const std::vector<ZTexture>& textures) : ZMaterial(0) { textures_ = textures; }

    void Initialize(std::shared_ptr<ZOFTree> root);

    static std::unique_ptr<ZMaterial> DefaultMaterial();

    void SetMeshID(const std::string& id) { meshId_ = id; }
    const std::string MeshID() const { return meshId_; }

    void SetProperties(const ZMaterialProperties& properties) { properties_ = properties; }
    const ZMaterialProperties& Properties() const { return properties_; }

    void SetAlpha(float alpha) { properties_.alpha = alpha; }
    float Alpha(float alpha) const { return properties_.alpha; }

    void AddTexture(const ZTexture& texture) { textures_.push_back(texture); }
    const std::vector<ZTexture>& Textures() const { return textures_; }

    int Index() const { return index_; }
    bool IsPBR() const { return isPBR_; }
    bool IsTextured() const { return !textures_.empty(); }
    bool HasDisplacement() const { return hasDisplacement_; }
    void SetPBR(bool pbr = true) { isPBR_ = pbr; }

protected:

    void HandleTextureReady(const std::shared_ptr<ZEvent>& event);

};
