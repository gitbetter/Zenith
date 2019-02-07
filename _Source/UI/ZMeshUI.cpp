//
//  ZMeshUI.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 06/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZMeshUI.hpp"

// void ZMeshUI::SetupUI() {
//   glGenBuffers(1, &vbo_);
//
//   glBindVertexArray(vao_);
//
//   // Bind the buffer object and set the vertex data
//   glBindBuffer(GL_ARRAY_BUFFER, vbo_);
//   glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(ZVertex), &vertices_[0], GL_STATIC_DRAW);
//
//   // Vertex position vector
//   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ZVertex), (void*)0);
//   glEnableVertexAttribArray(0);
//
//   // Vertex textures coordinates
//   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ZVertex), (void*)offsetof(ZVertex, uv));
//   glEnableVertexAttribArray(2);
//
//   glBindVertexArray(0);
// }
