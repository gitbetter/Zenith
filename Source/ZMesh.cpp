//
//  ZMesh.cpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ZMesh.hpp"
#include "ZShader.hpp"

ZMesh::ZMesh(std::vector<ZVertex> vertices, std::vector<unsigned int> indices, std::vector<ZTexture> textures) {
  vertices_ = vertices;
  indices_ = indices;
  textures_ = textures;

  Setup();
}

void ZMesh::Render(ZShader* shader) {

}

void ZMesh::Setup() {
  glGenVertexArrays(1, &arrayObj_);
  glGenBuffers(1, &bufferObj_);
  glGenBuffers(1, &elementBufferObj_);

  glBindVertexArray(arrayObj_);

  // Bind the buffer object and set the vertex data
  glBindBuffer(GL_ARRAY_BUFFER, bufferObj_);
  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(ZVertex), &vertices_[0], GL_STATIC_DRAW);

  //  Bind the element buffer object (for indexed drawing) and set the index data
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObj_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), &indices_[0], GL_STATIC_DRAW);

  // Vertex positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ZVertex), (void*)0);

  // Vertex normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ZVertex), (void*)offsetof(ZVertex, normal));

  // Vertex textures coordinates
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ZVertex), (void*)offsetof(ZVertex, uv));

  glBindVertexArray(0);
}
