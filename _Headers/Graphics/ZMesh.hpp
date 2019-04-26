/*
 
 ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
 /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
 \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
 ZMesh.hpp
 
 Created by Adrian Sanchez on 06/02/2019.
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
#include "ZProcess.hpp"

// Forward Declarations
class ZShader;
class aiScene;
class aiNode;

// Class and Data Structure Definitions
class ZMesh : public ZProcess {
    
public:
    
    virtual ~ZMesh() { }
    
    virtual void Render(ZShader* shader, ZMaterial* material) = 0;
    virtual ZMeshDrawStyle DrawStyle() const { return drawStyle_; };
    
    void SetAssimpScene(const aiScene* scene) { assimpScene_ = scene; }
    std::vector<glm::mat4> BoneTransform(float secondsTime);
    void ReadNodeHierarchy(float animTime, const aiNode* node, const glm::mat4& parentTransform);
    
protected:
    
    ZBufferData bufferData_;
    ZMeshDrawStyle drawStyle_;
    const aiScene* assimpScene_ = nullptr;
    std::map<std::string, unsigned int> bonesMap_;
    std::vector<ZBoneInfo> boneInfo_;
    
};
