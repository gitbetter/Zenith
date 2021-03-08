/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZSphere.hpp

    Created by Adrian Sanchez on 02/19/21.
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

 /* Sphere code adapted from http://www.songho.ca/opengl/gl_sphere.html */

#include "ZSphere.hpp"
#include "ZServices.hpp"

void ZSphere::Initialize()
{
    if (smooth_)
        BuildSmooth();
    else
        BuildFlat();

    ZModel::Initialize();
}

void ZSphere::Initialize(const std::shared_ptr<ZOFNode>& data)
{
    std::shared_ptr<ZOFObjectNode> node = std::dynamic_pointer_cast<ZOFObjectNode>(data);
    if (!node)
    {
        LOG("Could not initalize ZSphere: node data is invalid", ZSeverity::Error);
        return;
    }

    ZOFPropertyMap props = node->properties;

    if (props.find("radius") != props.end() && props["radius"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> radiusProp = props["radius"]->Value<ZOFNumber>(0);
        radius_ = radiusProp->value;
    }

    if (props.find("segments") != props.end() && props["segments"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> segmentsProp = props["segments"]->Value<ZOFNumberList>(0);
        segments_ = glm::vec2(segmentsProp->value[0], segmentsProp->value[1]);
    }

    if (props.find("isSmooth") != props.end() && props["isSmooth"]->HasValues())
    {
        std::shared_ptr<ZOFString> smoothProp = props["isSmooth"]->Value<ZOFString>(0);
        smooth_ = smoothProp->value == "Yes";
    }

    Initialize();
}

std::shared_ptr<ZSphere> ZSphere::Create(float radius, const glm::vec2& segments, bool smooth)
{
    auto sphere = std::make_shared<ZSphere>(radius, segments, smooth);
    sphere->Initialize();
    return sphere;
}

void ZSphere::BuildSmooth()
{
    ZVertex3DDataOptions options;

    float x, y, z, xy;                              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius_;    // normal
    float s, t;                                     // texCoord

    unsigned int sectorCount = segments_.x;
    unsigned int stackCount = segments_.y;

    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stackCount; ++i)
    {
        stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius_ * cosf(stackAngle);             // r * cos(u)
        z = radius_ * sinf(stackAngle);              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for (int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            options.vertices.emplace_back(
                glm::vec3(x, y, z),
                glm::vec3(x * lengthInv, y * lengthInv, z * lengthInv),
                glm::vec2((float)j / sectorCount, (float)i / stackCount)
            );
        }
    }

    // indices
    //  k1--k1+1
    //  |  / |
    //  | /  |
    //  k2--k2+1
    unsigned int k1, k2;
    for (int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding 1st and last stacks
            if (i != 0)
            {
                options.indices.insert(options.indices.end(),
                    { k1, k2, k1 + 1 }
                );
            }

            if (i != (stackCount - 1))
            {
                options.indices.insert(options.indices.end(),
                    { k1 + 1, k2, k2 + 1 }
                );
            }
        }
    }

    meshes_.clear();
    std::shared_ptr<ZMesh3D> mesh = std::make_shared<ZMesh3D>(options);
    mesh->Initialize();
    meshes_[mesh->ID()] = mesh;
}

void ZSphere::BuildFlat()
{
    ZVertex3DDataOptions options;

    ZVertex3DList tmpVertices;

    unsigned int sectorCount = segments_.x;
    unsigned int stackCount = segments_.y;

    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;

    // compute all vertices first, each vertex contains (x,y,z,s,t) except normal
    for (int i = 0; i <= stackCount; ++i)
    {
        stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        float xy = radius_ * cosf(stackAngle);       // r * cos(u)
        float z = radius_ * sinf(stackAngle);        // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for (int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            tmpVertices.emplace_back(
                glm::vec3(xy * cosf(sectorAngle), xy * sinf(sectorAngle), z),
                glm::vec3(0.f),
                glm::vec2((float)j / sectorCount, (float)i / stackCount)
            );
        }
    }

    ZVertex3D v1, v2, v3, v4;                       // 4 vertex positions and tex coords
    glm::vec3 n;                                    // 1 face normal

    int i, j, k, vi1, vi2;
    unsigned int index = 0;                         // index for vertex
    for (i = 0; i < stackCount; ++i)
    {
        vi1 = i * (sectorCount + 1);                // index of tmpVertices
        vi2 = (i + 1) * (sectorCount + 1);

        for (j = 0; j < sectorCount; ++j, ++vi1, ++vi2)
        {
            // get 4 vertices per sector
            //  v1--v3
            //  |    |
            //  v2--v4
            v1 = tmpVertices[vi1];
            v2 = tmpVertices[vi2];
            v3 = tmpVertices[vi1 + 1];
            v4 = tmpVertices[vi2 + 1];

            // if 1st stack and last stack, store only 1 triangle per sector
            // otherwise, store 2 triangles (quad) per sector
            if (i == 0) // a triangle for first stack ==========================
            {
                n = ZVertex3D::ComputeFaceNormal(v1, v2, v4);

                options.vertices.emplace_back(
                    v1.position,
                    n,
                    v1.uv
                );
                options.vertices.emplace_back(
                    v2.position,
                    n,
                    v2.uv
                );
                options.vertices.emplace_back(
                    v4.position,
                    n,
                    v4.uv
                );

                options.indices.insert(options.indices.end(),
                    { index, index + 1, index + 2 }
                );

                index += 3;     // for next
            }
            else if (i == (stackCount - 1)) // a triangle for last stack =========
            {
                n = ZVertex3D::ComputeFaceNormal(v1, v2, v3);

                options.vertices.emplace_back(
                    v1.position,
                    n,
                    v1.uv
                );
                options.vertices.emplace_back(
                    v2.position,
                    n,
                    v2.uv
                );
                options.vertices.emplace_back(
                    v3.position,
                    n,
                    v3.uv
                );

                options.indices.insert(options.indices.end(),
                    { index, index + 1, index + 2 }
                );

                index += 3;     // for next
            }
            else // 2 triangles for others ====================================
            {
                n = ZVertex3D::ComputeFaceNormal(v1, v2, v3);

                options.vertices.emplace_back(
                    v1.position,
                    n,
                    v1.uv
                );
                options.vertices.emplace_back(
                    v2.position,
                    n,
                    v2.uv
                );
                options.vertices.emplace_back(
                    v3.position,
                    n,
                    v3.uv
                );
                options.vertices.emplace_back(
                    v4.position,
                    n,
                    v4.uv
                );

                options.indices.insert(options.indices.end(),
                    { index, index + 1, index + 2,
                      index + 2, index + 1, index + 3 }
                );

                index += 4;     // for next
            }
        }
    }

    meshes_.clear();
    std::shared_ptr<ZMesh3D> mesh = std::make_shared<ZMesh3D>(options);
    mesh->Initialize();
    meshes_[mesh->ID()] = mesh;
}
