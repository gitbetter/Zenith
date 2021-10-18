/*

     ______     ______     __   __     __     ______   __  __
    /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
    \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
        /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
        \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

        ZBVH.hpp

        Created by Adrian Sanchez on 02/24/2021.
        Copyright � 2019 Pervasive Sense. All rights reserved.

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
#include "ZAABBox.hpp"
#include "ZRay.hpp"

enum class ZBVHSplitMethod
{
    SAH, Middle, EqualCounts
};

struct ZBVHPrimitive
{
    ZHGameObject objectHandle;
    ZAABBox bounds;

    ZBVHPrimitive(const ZHGameObject& object, const ZAABBox& bounds) : objectHandle(object), bounds(bounds) { }
};

struct ZBVHPrimitiveInfo
{
    int index;
    ZAABBox bounds;
    glm::vec3 centroid;

    ZBVHPrimitiveInfo() { }
    ZBVHPrimitiveInfo(int index, const ZAABBox& bounds)
        : index(index), bounds(bounds),
          centroid(0.5f * bounds.minimum + 0.5f * bounds.maximum) { }
};

struct ZBVHBuildNode
{
    ZAABBox bounds;
    std::array<std::shared_ptr<ZBVHBuildNode>, 2> children;
    int splitAxis, firstPrimitiveOffset, primitivesCount;

    ZBVHBuildNode(int first, int numPrimitives, const ZAABBox& bounds)
        : firstPrimitiveOffset(first), primitivesCount(numPrimitives), bounds(bounds)
    {
        children.fill(nullptr);
    }

    ZBVHBuildNode(int axis, const std::shared_ptr<ZBVHBuildNode>& c0, const std::shared_ptr<ZBVHBuildNode>& c1)
    {
        children[0] = c0;
        children[1] = c1;
        bounds = ZAABBox::Union(c0->bounds, c1->bounds);
        splitAxis = axis;
        primitivesCount = 0;
    }
};

struct ZIntersectHitResult
{
    ZHGameObject objectHandle;
};

struct ZLinearBVHNode
{
    ZAABBox bounds;
    union {
        int primitiveOffset;
        int secondChildOffset;
    };
    uint16_t primitiveCount;
    uint8_t axis;
    uint8_t pad[1]; // cache-line align to 32-bytes
};

// Class and Data Structure Definitions
class ZBVH
{

public:

    ZBVH::ZBVH(int maxNodePrimitives, ZBVHSplitMethod splitMethod)
        : maxNodePrimitives_(std::min(255, maxNodePrimitives)), splitMethod_(splitMethod)
    { }
    ~ZBVH() { }

    void Build();
    void AddPrimitive(const ZBVHPrimitive& primitive);
    bool Intersect(ZRay& ray, ZIntersectHitResult& hitResult);

protected:

    std::vector<ZBVHPrimitive> userPrimitives_;
    std::vector<ZBVHPrimitive> primitives_;
    unsigned int maxNodePrimitives_;
    ZBVHSplitMethod splitMethod_;
    std::vector<ZLinearBVHNode> nodes_;

    std::shared_ptr<ZBVHBuildNode> RecursiveBuild(std::vector<ZBVHPrimitiveInfo>& primitiveInfo, int start, int end, int* totalNodes, std::vector<ZBVHPrimitive>& orderedPrimitives);
    std::shared_ptr<ZBVHBuildNode> CreateLeafNode(std::vector<ZBVHPrimitive>& orderedPrimitives, int start, int end, std::vector<ZBVHPrimitiveInfo>& primitiveInfo, int primitiveCount, ZAABBox bounds);
    int FlattenBVHTree(const std::shared_ptr<ZBVHBuildNode>& node, int* offset);

};