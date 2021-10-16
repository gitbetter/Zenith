/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZCylinder.hpp

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

/* Cylinder code adapted from http://www.songho.ca/opengl/gl_cylinder.html */

#include "ZCylinder.hpp"
#include "ZServices.hpp"

void ZCylinder::OnCreate()
{
	if (smooth_)
	{
		BuildSmooth();
	}
	else
	{
		BuildFlat();
	}
}

void ZCylinder::OnDeserialize(const std::shared_ptr<ZOFObjectNode>& dataNode)
{
    if (!dataNode)
    {
        LOG("Could not initalize ZCylinder: node data is invalid", ZSeverity::Error);
        return;
    }

    ZOFPropertyMap props = dataNode->properties;

    if (props.find("topRadius") != props.end() && props["topRadius"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> topRadiusProp = props["topRadius"]->Value<ZOFNumber>(0);
        topRadius_ = topRadiusProp->value;
    }

    if (props.find("baseRadius") != props.end() && props["baseRadius"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> baseRadiusProp = props["baseRadius"]->Value<ZOFNumber>(0);
        baseRadius_ = baseRadiusProp->value;
    }

    if (props.find("height") != props.end() && props["height"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> heightRadiusProp = props["height"]->Value<ZOFNumber>(0);
        height_ = heightRadiusProp->value;
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

    if (smooth_)
    {
		BuildSmooth();
    }
    else
    {
		BuildFlat();
    }
}

std::vector<glm::vec3> ZCylinder::GetSideNormals()
{
    float sectorStep = 2 * PI / segments_.x;
    float sectorAngle;  // radian

    // compute the normal vector at 0 degree first
    // tanA = (baseRadius-topRadius) / height
    float zAngle = atan2(baseRadius_ - topRadius_, height_);
    float x0 = cos(zAngle);     // nx
    float y0 = 0;               // ny
    float z0 = sin(zAngle);     // nz

    // rotate (x0,y0,z0) per sector angle
    std::vector<glm::vec3> normals;
    for (int i = 0; i <= segments_.x; ++i)
    {
        sectorAngle = i * sectorStep;
        normals.emplace_back(
            cos(sectorAngle) * x0 - sin(sectorAngle) * y0,
            sin(sectorAngle) * x0 + cos(sectorAngle) * y0,
            z0
        );
    }

    return normals;
}

void ZCylinder::BuildSmooth()
{
    ZVertex3DDataOptions options;

    int sectorCount = segments_.x;
    int stackCount = segments_.y;

    float z;
    float radius;                                   // radius for each stack

    // get normals for cylinder sides
    ZVertex2DList unitCircleVertices = ZVertex2D::GenerateUnitCircleVertices(segments_.x);
    std::vector<glm::vec3> sideNormals = GetSideNormals();

    // put vertices of side cylinder to array by scaling unit circle
    for (int i = 0; i <= segments_.y; ++i)
    {
        z = -(height_ * 0.5f) + (float)i / stackCount * height_;      // vertex position z
        radius = baseRadius_ + (float)i / stackCount * (topRadius_ - baseRadius_);     // lerp
        float t = 1.0f - (float)i / stackCount;   // top-to-bottom

        for (unsigned int j = 0; j <= sectorCount; ++j)
        {
            auto unitCircleVertex = unitCircleVertices[j].vertex;
            options.vertices.emplace_back(
                glm::vec3(unitCircleVertex.x * radius, unitCircleVertex.y * radius, z),
                sideNormals[j],
                glm::vec2((float)j / sectorCount, t)
            );
        }
    }

    // remember where the base.top vertices start
    unsigned int baseVertexIndex = options.vertices.size();

    // put vertices of base of cylinder
    z = -height_ * 0.5f;
    options.vertices.emplace_back(
        glm::vec3(0.f, 0.f, z),
        glm::vec3(0.f, 0.f, -1.f),
        glm::vec2(0.5f, 0.5f)
    );
    for (int i = 0; i < sectorCount; ++i)
    {
        auto unitCircleVertex = unitCircleVertices[i].vertex;
        options.vertices.emplace_back(
            glm::vec3(unitCircleVertex.x * baseRadius_, unitCircleVertex.y * baseRadius_, z),
            glm::vec3(0.f, 0.f, -1.f),
            glm::vec2(-unitCircleVertex.x * 0.5f + 0.5f, -unitCircleVertex.y * 0.5f + 0.5f)
        );
    }

    // remember where the base vertices start
    unsigned int topVertexIndex = options.vertices.size();

    // put vertices of top of cylinder
    z = height_ * 0.5f;
    options.vertices.emplace_back(
        glm::vec3(0.f, 0.f, z),
        glm::vec3(0.f, 0.f, 1.f),
        glm::vec2(0.5f, 0.5f)
    );
    for (int i = 0; i < sectorCount; ++i)
    {
        auto unitCircleVertex = unitCircleVertices[i].vertex;
        options.vertices.emplace_back(
            glm::vec3(unitCircleVertex.x * topRadius_, unitCircleVertex.y * topRadius_, z),
            glm::vec3(0.f, 0.f, 1.f),
            glm::vec2(unitCircleVertex.x * 0.5f + 0.5f, -unitCircleVertex.y * 0.5f + 0.5f)
        );
    }

    // put indices for sides
    unsigned int k1, k2;
    for (int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);     // bebinning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            // 2 trianles per sector
            options.indices.insert(options.indices.end(),
                { k1, k1 + 1, k2,
                  k2, k1 + 1, k2 + 1 }
            );
        }
    }

    // put indices for base
    for (unsigned int i = 0, k = baseVertexIndex + 1; i < sectorCount; ++i, ++k)
    {
        if (i < (sectorCount - 1))
            options.indices.insert(options.indices.end(),
                { baseVertexIndex, k + 1, k }
        );
        else    // last triangle
            options.indices.insert(options.indices.end(),
                { baseVertexIndex, baseVertexIndex + 1, k }
        );
    }

    for (unsigned int i = 0, k = topVertexIndex + 1; i < sectorCount; ++i, ++k)
    {
        if (i < (sectorCount - 1))
            options.indices.insert(options.indices.end(),
                { topVertexIndex, k, k + 1 }
        );
        else
            options.indices.insert(options.indices.end(),
                { topVertexIndex, k, topVertexIndex + 1 }
        );
    }

    meshes.clear();
    ZMesh3D mesh(options);
    mesh.Initialize();
    meshes.emplace_back(mesh);
}

void ZCylinder::BuildFlat()
{
    ZVertex3DDataOptions options;

    ZVertex3DList tmpVertices;

    unsigned int i, j, k;    // indices
    float z, s, t, radius;

    int sectorCount = segments_.x;
    int stackCount = segments_.y;

    ZVertex2DList unitCircleVertices = ZVertex2D::GenerateUnitCircleVertices(segments_.x);

    // put tmp vertices of cylinder side to array by scaling unit circle
    //NOTE: start and end vertex positions are same, but texcoords are different
    //      so, add additional vertex at the end point
    for (i = 0; i <= stackCount; ++i)
    {
        z = -(height_ * 0.5f) + (float)i / stackCount * height_;      // vertex position z
        radius = baseRadius_ + (float)i / stackCount * (topRadius_ - baseRadius_);     // lerp
        t = 1.0f - (float)i / stackCount;   // top-to-bottom

        for (j = 0; j <= sectorCount; ++j)
        {
            auto unitCircleVertex = unitCircleVertices[j].vertex;
            s = (float)j / sectorCount;

            tmpVertices.emplace_back(
                glm::vec3(unitCircleVertex.x * radius, unitCircleVertex.y * radius, z),
                glm::vec3(0.f),
                glm::vec2(s, t)
            );
        }
    }

    ZVertex3D v1, v2, v3, v4;      // 4 vertex positions v1, v2, v3, v4
    glm::vec3 n;       // 1 face normal
    int vi1, vi2;               // indices
    unsigned int index = 0;

    // v2-v4 <== stack at i+1
    // | \ |
    // v1-v3 <== stack at i
    for (i = 0; i < stackCount; ++i)
    {
        vi1 = i * (sectorCount + 1);            // index of tmpVertices
        vi2 = (i + 1) * (sectorCount + 1);

        for (j = 0; j < sectorCount; ++j, ++vi1, ++vi2)
        {
            v1 = tmpVertices[vi1];
            v2 = tmpVertices[vi2];
            v3 = tmpVertices[vi1 + 1];
            v4 = tmpVertices[vi2 + 1];

            // compute a face normal of v1-v3-v2
            n = ZVertex3D::ComputeFaceNormal(v1, v3, v2);

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
                { index, index + 2, index + 1,
                    index + 1, index + 2, index + 3 }
            );

            index += 4;     // for next
        }
    }

    unsigned int baseVertexIndex = options.vertices.size();

    // put vertices of base of cylinder
    z = -height_ * 0.5f;
    options.vertices.emplace_back(
        glm::vec3(0.f, 0.f, z),
        glm::vec3(0.f, 0.f, -1.f),
        glm::vec2(0.5f, 0.5f)
    );
    for (i = 0; i < sectorCount; ++i)
    {
        auto unitCircleVertex = unitCircleVertices[i].vertex;
        options.vertices.emplace_back(
            glm::vec3(unitCircleVertex.x * baseRadius_, unitCircleVertex.y * baseRadius_, z),
            glm::vec3(0.f, 0.f, -1.f),
            glm::vec2(-unitCircleVertex.x * 0.5f + 0.5f, -unitCircleVertex.y * 0.5f + 0.5f) // flip horizontal
        );
    }

    // put indices for base
    for (i = 0, k = baseVertexIndex + 1; i < sectorCount; ++i, ++k)
    {
        if (i < sectorCount - 1)
            options.indices.insert(options.indices.end(),
                { baseVertexIndex, k + 1, k }
            );
        else
            options.indices.insert(options.indices.end(),
                { baseVertexIndex, baseVertexIndex + 1, k }
            );
    }

    unsigned int topVertexIndex = options.vertices.size();

    // put vertices of top of cylinder
    z = height_ * 0.5f;
    options.vertices.emplace_back(
        glm::vec3(0.f, 0.f, z),
        glm::vec3(0.f, 0.f, 1.f),
        glm::vec2(0.5f, 0.5f)
    );
    for (i = 0; i < sectorCount; ++i)
    {
        auto unitCircleVertex = unitCircleVertices[i].vertex;
        options.vertices.emplace_back(
            glm::vec3(unitCircleVertex.x* topRadius_, unitCircleVertex.y* topRadius_, z),
            glm::vec3(0.f, 0.f, 1.f),
            glm::vec2(unitCircleVertex.x * 0.5f + 0.5f, -unitCircleVertex.y * 0.5f + 0.5f)
        );
    }

    for (i = 0, k = topVertexIndex + 1; i < sectorCount; ++i, ++k)
    {
        if (i < sectorCount - 1)
            options.indices.insert(options.indices.end(),
                { topVertexIndex, k, k + 1 }
            );
        else
            options.indices.insert(options.indices.end(),
                { topVertexIndex, k, topVertexIndex + 1 }
            );
    }

	meshes.clear();
	ZMesh3D mesh(options);
	mesh.Initialize();
	meshes.emplace_back(mesh);
}
