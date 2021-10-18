/*

     ______     ______     __   __     __     ______   __  __
    /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
    \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
        /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
        \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

        ZBVH.cpp

        Created by Adrian Sanchez on 02/24/2021.
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

#include "ZBVH.hpp"

void ZBVH::Build()
{
    primitives_.swap(userPrimitives_);
    userPrimitives_.clear();

    if (primitives_.empty()) return;

    std::vector<ZBVHPrimitiveInfo> primitiveInfo(primitives_.size());
    for (auto i = 0; i < primitives_.size(); ++i) {
        primitiveInfo[i] = { i, primitives_[i].bounds };
    }

    int totalNodes = 0;
    std::vector<ZBVHPrimitive> orderedPrimitives;
    orderedPrimitives.reserve(primitives_.size());
    std::shared_ptr<ZBVHBuildNode> root = RecursiveBuild(primitiveInfo, 0, primitives_.size(), &totalNodes, orderedPrimitives);
    primitives_.swap(orderedPrimitives);

    int offset = 0; 
    nodes_.resize(totalNodes);
    FlattenBVHTree(root, &offset);
}

void ZBVH::AddPrimitive(const ZBVHPrimitive& primitive)
{
    userPrimitives_.emplace_back(primitive);
}

bool ZBVH::Intersect(ZRay& ray, ZIntersectHitResult& hitResult)
{
    if (primitives_.empty()) return false;

    bool hit = false;
    glm::vec3 invDir = 1.f / ray.Direction();
    int dirIsNegative[3] = { invDir.x < 0, invDir.y < 0, invDir.z < 0 };

    int toVisitOffset = 0, currentNodeIndex = 0;
    int nodesToVisit[64];
    std::map<int, ZHGameObject> hits;
    while (true) {
        ZLinearBVHNode& node = nodes_[currentNodeIndex];
        if (node.bounds.Intersects(ray)) {
            if (node.primitiveCount > 0) {
                for (auto i = 0; i < node.primitiveCount; ++i) {
                    auto& prim = primitives_[static_cast<size_t>(node.primitiveOffset) + i];
                    if (prim.bounds.Intersects(ray) && ray.tMax > 0) {
                        hits[ray.tMax] = prim.objectHandle;
                        hitResult.objectHandle = prim.objectHandle;
                        hit = true;
                    }
                }
                if (toVisitOffset == 0) break;
                currentNodeIndex = nodesToVisit[--toVisitOffset];
            }
            else {
                if (dirIsNegative[node.axis]) {
                    nodesToVisit[toVisitOffset++] = currentNodeIndex + 1;
                    currentNodeIndex = node.secondChildOffset;
                }
                else {
                    nodesToVisit[toVisitOffset++] = node.secondChildOffset;
                    currentNodeIndex += 1;
                }
            }
        }
        else {
            if (toVisitOffset == 0) break;
            currentNodeIndex = nodesToVisit[--toVisitOffset];
        }
    }

    if (!hits.empty())
    {
        hitResult.objectHandle = hits.begin()->second;
    }

    return hit;
}

std::shared_ptr<ZBVHBuildNode> ZBVH::RecursiveBuild(std::vector<ZBVHPrimitiveInfo>& primitiveInfo, int start, int end, int* totalNodes, std::vector<ZBVHPrimitive>& orderedPrimitives)
{
    assert(start != end);
    (*totalNodes)++;
    ZAABBox bounds;
    for (auto i = start; i < end; ++i) {
        bounds = ZAABBox::Union(bounds, primitiveInfo[i].bounds);
    }
    int primitiveCount = end - start;
    if (primitiveCount == 1) {
        return CreateLeafNode(orderedPrimitives, start, end, primitiveInfo, primitiveCount, bounds);
    }
    else {
        ZAABBox centroidBounds;
        for (auto i = start; i < end; ++i) {
            centroidBounds = ZAABBox::Union(centroidBounds, primitiveInfo[i].centroid);
        }
        int axis = centroidBounds.MaxExtent();
        int mid = (start + end) / 2;
        if (centroidBounds.maximum[axis] == centroidBounds.minimum[axis]) {
            return CreateLeafNode(orderedPrimitives, start, end, primitiveInfo, primitiveCount, bounds);
        }
        else {
            switch (splitMethod_) {
            case ZBVHSplitMethod::Middle: {
                float midPartition = (centroidBounds.minimum[axis] + centroidBounds.maximum[axis]) / 2.f;
                ZBVHPrimitiveInfo* midPtr = std::partition(&primitiveInfo[start], &primitiveInfo[end - 1] + 1,
                    [axis, midPartition](const ZBVHPrimitiveInfo& pi) {
                        return pi.centroid[axis] < midPartition;
                    });
                mid = midPtr - &primitiveInfo[0];
                if (mid != start && mid != end)
                    break;
            }
            case ZBVHSplitMethod::EqualCounts: {
                mid = (start + end) / 2;
                std::nth_element(&primitiveInfo[start], &primitiveInfo[mid], &primitiveInfo[end - 1] + 1,
                    [axis](const ZBVHPrimitiveInfo& a, const ZBVHPrimitiveInfo& b) {
                        return a.centroid[axis] < b.centroid[axis];
                    });
                break;
            }
            case ZBVHSplitMethod::SAH: {
                if (primitiveCount <= 2) {
                    mid = (start + end) / 2;
                    std::nth_element(&primitiveInfo[start], &primitiveInfo[mid], &primitiveInfo[end - 1] + 1,
                        [axis](const ZBVHPrimitiveInfo& a, const ZBVHPrimitiveInfo& b) {
                            return a.centroid[axis] < b.centroid[axis];
                        });
                }
                else {
                    constexpr int numBuckets = 12;
                    struct BucketInfo {
                        int count = 0;
                        ZAABBox bounds;
                    };
                    BucketInfo buckets[numBuckets];

                    for (auto i = start; i < end; ++i) {
                        int b = numBuckets * centroidBounds.Offset(primitiveInfo[i].centroid)[axis];
                        if (b == numBuckets) b = numBuckets - 1;
                        buckets[b].count++;
                        buckets[b].bounds = ZAABBox::Union(buckets[b].bounds, primitiveInfo[i].bounds);
                    }

                    float cost[numBuckets - 1];
                    for (auto i = 0; i < numBuckets - 1; ++i) {
                        ZAABBox b0, b1;
                        int count0 = 0, count1 = 0;
                        for (auto j = 0; j <= i; ++j) {
                            b0 = ZAABBox::Union(b0, buckets[j].bounds);
                            count0 += buckets[j].count;
                        }
                        for (auto j = i + 1; j < numBuckets; ++j) {
                            b1 = ZAABBox::Union(b1, buckets[j].bounds);
                            count1 += buckets[j].count;
                        }
                        cost[i] = 1 + (count0 * b0.SurfaceArea() + count1 * b1.SurfaceArea()) / bounds.SurfaceArea();
                    }

                    float minCost = cost[0];
                    int minCostBucket = 0;
                    for (auto i = 1; i < numBuckets - 1; ++i) {
                        if (cost[i] < minCost) {
                            minCost = cost[i];
                            minCostBucket = i;
                        }
                    }

                    float leafCost = primitiveCount;
                    if (primitiveCount > maxNodePrimitives_ || minCost < leafCost) {
                        ZBVHPrimitiveInfo* midInfo = std::partition(&primitiveInfo[start], &primitiveInfo[end - 1] + 1,
                            [=](const ZBVHPrimitiveInfo& pi) {
                                int b = numBuckets * centroidBounds.Offset(pi.centroid)[axis];
                                if (b == numBuckets) b = numBuckets - 1;
                                return b <= minCostBucket;
                            });
                        mid = midInfo - &primitiveInfo[0];
                    }
                    else {
                        return CreateLeafNode(orderedPrimitives, start, end, primitiveInfo, primitiveCount, bounds);
                    }

                    // Default to middle split if partition is still not generous enough
                    if (mid == start || mid == end) {
                        mid = (start + end) / 2;
                        std::nth_element(&primitiveInfo[start], &primitiveInfo[mid], &primitiveInfo[end - 1] + 1,
                            [axis](const ZBVHPrimitiveInfo& a, const ZBVHPrimitiveInfo& b) {
                                return a.centroid[axis] < b.centroid[axis];
                            });
                    }
                }
                break;
            }
            }

            return std::make_shared<ZBVHBuildNode>(
                axis, 
                start == mid ? nullptr : RecursiveBuild(primitiveInfo, start, mid, totalNodes, orderedPrimitives),
                mid == end ? nullptr : RecursiveBuild(primitiveInfo, mid, end, totalNodes, orderedPrimitives)
            );
        }
    }
}

std::shared_ptr<ZBVHBuildNode> ZBVH::CreateLeafNode(std::vector<ZBVHPrimitive>& orderedPrimitives, int start, int end, std::vector<ZBVHPrimitiveInfo>& primitiveInfo, int primitiveCount, ZAABBox bounds)
{
    int firstPrimitiveOffset = orderedPrimitives.size();
    for (auto i = start; i < end; ++i) {
        int primitiveIndex = primitiveInfo[i].index;
        orderedPrimitives.push_back(primitives_[primitiveIndex]);
    }
    return std::make_shared<ZBVHBuildNode>(firstPrimitiveOffset, primitiveCount, bounds);
}

int ZBVH::FlattenBVHTree(const std::shared_ptr<ZBVHBuildNode>& node, int* offset)
{
    ZLinearBVHNode& linearNode = nodes_[*offset];
    linearNode.bounds = node->bounds;
    int thisOffset = (*offset)++;
    if (node->primitivesCount > 0) {
        linearNode.primitiveOffset = node->firstPrimitiveOffset;
        linearNode.primitiveCount = node->primitivesCount;
    }
    else {
        linearNode.axis = node->splitAxis;
        linearNode.primitiveCount = 0;
        FlattenBVHTree(node->children[0], offset);
        linearNode.secondChildOffset = FlattenBVHTree(node->children[1], offset);
    }
    return thisOffset;
}
