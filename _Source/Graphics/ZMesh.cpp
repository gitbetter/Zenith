/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZMesh.cpp

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

#include "ZMesh.hpp"
#include <assimp/scene.h>

void ZVertexBoneData::AddBoneData(unsigned int bone, float weight) {
    for (unsigned int i = 0; i < BONES_PER_VERTEX; i++) {
        if (weights[i] == 0.f) {
            ids[i] = bone;
            weights[i] = weight;
            return;
        }
    }
}

std::vector<glm::mat4> ZMesh::BoneTransform(float secondsTime) {
    glm::mat4 identity(1.f);
    std::vector<glm::mat4> transforms;
    
    // TODO: Parametrize animation index
    float ticksPerSecond = assimpScene_->mAnimations[0]->mTicksPerSecond != 0 ? assimpScene_->mAnimations[0]->mTicksPerSecond : 25.f;
    float timeInTicks = secondsTime * ticksPerSecond;
    float animationTime = fmod(timeInTicks, assimpScene_->mAnimations[0]->mDuration);
    
    ReadNodeHierarchy(animationTime, assimpScene_->mRootNode, identity);
    
    for (unsigned int i = 0; i < boneInfo_.size(); i++) {
        transforms.push_back(boneInfo_[i].transformation);
    }
    
    return transforms;
}

void ZMesh::ReadNodeHierarchy(float animTime, const aiNode* node, const glm::mat4& parentTransform) {
    const aiAnimation* animation = assimpScene_->mAnimations[0];
    glm::mat4 nodeTransform = ASSIMP_TO_GLM_MAT4(node->mTransformation);
    std::string nodeName(node->mName.data);
    
    const aiNodeAnim* nodeAnimation = nullptr;
    for (unsigned int i = 0; i < animation->mNumChannels; i++) {
        const aiNodeAnim* anim = animation->mChannels[i];
        if (std::string(anim->mNodeName.data) == nodeName) {
            nodeAnimation = anim;
        }
    }
    
    if (nodeAnimation) {
        
    }
}
