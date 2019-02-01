//
//  ZMesh.cpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZCamera.hpp"
#include "ZMesh.hpp"
#include "ZShader.hpp"
#include "ZMaterial.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// TODO: Create enums for OGL layout attributes and magic numbers

ZMesh::ZMesh(std::vector<ZVertex> vertices, std::vector<unsigned int> indices, ZMaterial material)
: vertices_(vertices), indices_(indices), material_(material) {
  Setup();
}

void ZMesh::Render(ZShader* shader) {
  // Activate the texture for use
  shader->Activate();

  // Loop through as many textures as we have and bind
  // the corresponding textures (preloaded) and texture samplers (shader side uniforms)
  unsigned int diffuseMaps = 1, specularMaps = 1, normalMaps = 1, heightMaps = 1;
  for (unsigned int i = 0; i < material_.Textures().size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    std::string textureNumber, textureName = material_.Textures()[i].type;
    if (textureName == "diffuse") textureNumber = std::to_string(diffuseMaps++);
    else if (textureName == "specular") textureNumber = std::to_string(specularMaps++);
    else if (textureName == "normal") textureNumber = std::to_string(normalMaps++);
    else if (textureName == "height") textureNumber = std::to_string(heightMaps++);

    // Set the texture sampler to the correct unit and bind the texture
    glUniform1i(glGetUniformLocation(shader->GetID(), (textureName + textureNumber).c_str()), i);
    glBindTexture(GL_TEXTURE_2D, material_.Textures()[i].id);
  }

  // Draw the mesh
  glBindVertexArray(vao_);
  glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, 0);

  // Clean things up
  glBindVertexArray(0);
  glActiveTexture(GL_TEXTURE0);
}

void ZMesh::Setup() {
  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);
  glGenBuffers(1, &ebo_);

  glBindVertexArray(vao_);

  // Bind the buffer object and set the vertex data
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(ZVertex), &vertices_[0], GL_STATIC_DRAW);

  //  Bind the element buffer object (for indexed drawing) and set the index data
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), &indices_[0], GL_STATIC_DRAW);

  // Vertex position vector
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ZVertex), (void*)0);
  glEnableVertexAttribArray(0);

  // Vertex normal vector
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ZVertex), (void*)offsetof(ZVertex, normal));
  glEnableVertexAttribArray(1);

  // Vertex textures coordinates
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ZVertex), (void*)offsetof(ZVertex, uv));
  glEnableVertexAttribArray(2);

  // Vertex tangent vector
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ZVertex), (void*)offsetof(ZVertex, tangent));
  glEnableVertexAttribArray(3);

  // Vertex bitangent vector (the tangent to the tangent, but not the normal)
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(ZVertex), (void*)offsetof(ZVertex, bitangent));
  glEnableVertexAttribArray(4);

  glBindVertexArray(0);
}
