/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
	/\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
	\/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

	ZGLBuffer.cpp

	Created by Adrian Sanchez on 27/01/2021.
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

#include "ZGLBuffer.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

void ZGLBuffer::Load(const ZVertex3DDataOptions& options)
{
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ivbo_);

    if (!options.indices.empty())
    {
        glGenBuffers(1, &ebo_);
    }

    glBindVertexArray(vao_);

    // Bind the buffer object and set the vertex data
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, options.vertices.size() * sizeof(ZVertex3D), &options.vertices[0], GL_STATIC_DRAW);

    if (!options.indices.empty())
    {
        //  Bind the element buffer object (for indexed drawing) and set the index data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, options.indices.size() * sizeof(unsigned int), &options.indices[0], GL_STATIC_DRAW);
    }

    // Vertex position vector
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ZVertex3D), (void*)0);
    glEnableVertexAttribArray(0);

    // Vertex normal vector
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ZVertex3D), (void*)offsetof(ZVertex3D, normal));
    glEnableVertexAttribArray(1);

    // Vertex textures coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ZVertex3D), (void*)offsetof(ZVertex3D, uv));
    glEnableVertexAttribArray(2);

    // Vertex tangent vector
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ZVertex3D), (void*)offsetof(ZVertex3D, tangent));
    glEnableVertexAttribArray(3);

    // Vertex bitangent vector (the tangent to the tangent, but not the normal)
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(ZVertex3D), (void*)offsetof(ZVertex3D, bitangent));
    glEnableVertexAttribArray(4);

    // Vertex bone id data
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(ZVertex3D), (void*)offsetof(ZVertex3D, boneIDs));
    glEnableVertexAttribArray(5);

    // Vertex bone weight data
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(ZVertex3D), (void*)offsetof(ZVertex3D, boneWeights));
    glEnableVertexAttribArray(6);

    // Instanced translation data
    glBindBuffer(GL_ARRAY_BUFFER, ivbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * options.instanced.count, options.instanced.translations.empty() ? NULL : &options.instanced.translations[0], GL_DYNAMIC_DRAW);

    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(9);
    glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(10);

    glVertexAttribDivisor(7, 1);
    glVertexAttribDivisor(8, 1);
    glVertexAttribDivisor(9, 1);
    glVertexAttribDivisor(10, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void ZGLBuffer::Bind()
{
    glBindVertexArray(vao_);
    if (ebo_ != 0) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    }
}

void ZGLBuffer::Unbind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void ZGLBuffer::Load(const ZVertex2DDataOptions& options)
{
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ivbo_);

    glBindVertexArray(vao_);

    // Bind the buffer object and set the vertex data
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    if (options.vertices.empty())
    {
        glBufferData(GL_ARRAY_BUFFER, sizeof(ZVertex2D) * options.numVertices, NULL, GL_STATIC_DRAW);
    }
    else
    {
        glBufferData(GL_ARRAY_BUFFER, options.vertices.size() * sizeof(ZVertex2D), &options.vertices[0], GL_STATIC_DRAW);
    }

    // Vertex position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ZVertex2D), (void*)0);
    glEnableVertexAttribArray(0);

    // Vertex texture coordinates attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ZVertex2D), (void*)offsetof(ZVertex2D, uv));
    glEnableVertexAttribArray(1);

    // Instanced translation data
    glBindBuffer(GL_ARRAY_BUFFER, ivbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * options.instanced.count, options.instanced.translations.empty() ? NULL : &options.instanced.translations[0], GL_DYNAMIC_DRAW);

    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(5);

    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void ZGLBuffer::Update(const ZVertex3DDataOptions& vertexData)
{
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, vertexData.vertices.size() * sizeof(ZVertex3D), &vertexData.vertices[0], GL_STATIC_DRAW);
    if (vertexData.instanced.count > 1)
    {
        glBindBuffer(GL_ARRAY_BUFFER, ivbo_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * vertexData.instanced.count, vertexData.instanced.translations.empty() ? NULL : &vertexData.instanced.translations[0], GL_DYNAMIC_DRAW);
    }
    if (!vertexData.indices.empty())
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexData.indices.size() * sizeof(unsigned int), &vertexData.indices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void ZGLBuffer::Update(const ZVertex2DDataOptions& vertexData)
{
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, vertexData.vertices.size() * sizeof(ZVertex2D), &vertexData.vertices[0], GL_DYNAMIC_DRAW);
    if (vertexData.instanced.count > 1)
    {
        glBindBuffer(GL_ARRAY_BUFFER, ivbo_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * vertexData.instanced.count, vertexData.instanced.translations.empty() ? NULL : &vertexData.instanced.translations[0], GL_DYNAMIC_DRAW);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void ZGLBuffer::Delete()
{
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
    glDeleteBuffers(1, &ebo_);
    glDeleteBuffers(1, &ivbo_);
}
