//
//  ZGLGraphicsStrategy.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 07/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ZEngine.hpp"
#include "ZGLGraphicsStrategy.hpp"
#include "ZDomain.hpp"
#include "ZDomainStrategy.hpp"
#include "ZLogger.hpp"

void ZGLGraphicsStrategy::Initialize() {
  glfwSetErrorCallback(GLFWErrorCallback);

  EnableDepthTesting();
  EnableStencilTesting();
}

void ZGLGraphicsStrategy::ClearViewport() {
  glClearColor(0.3f, 0.1f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glStencilMask(0x00);
}

void ZGLGraphicsStrategy::SwapBuffers() {
  GLFWwindow* window = glfwGetCurrentContext();
  glfwSwapBuffers(window);
}

void ZGLGraphicsStrategy::EnableDepthTesting() {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
}

void ZGLGraphicsStrategy::EnableStencilTesting() {
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
}

void ZGLGraphicsStrategy::EnableStencilBuffer() {
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  glStencilMask(0xFF);
}

void ZGLGraphicsStrategy::DisableStencilBuffer() {
  glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
  glStencilMask(0x00);
}

void ZGLGraphicsStrategy::EnableAlphaBlending() {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ZGLGraphicsStrategy::DisableAlphaBlending() {
  glDisable(GL_BLEND);
}

void ZGLGraphicsStrategy::BindFramebuffer(unsigned int frameBuffer) {
  glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
}

void ZGLGraphicsStrategy::UnbindFramebuffer() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ZGLGraphicsStrategy::BindTexture(ZTexture texture, unsigned int index) {
  glActiveTexture(GL_TEXTURE0 + index);
  if (texture.type == "cubemap") {
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture.id);
  } else {
    glBindTexture(GL_TEXTURE_2D, texture.id);
  }
}

ZBufferData ZGLGraphicsStrategy::LoadVertexData(std::vector<ZVertex> vertices, std::vector<unsigned int> indices) {
  ZBufferData bufferData;
  glGenVertexArrays(1, &bufferData.vao);
  glGenBuffers(1, &bufferData.vbo);
  glGenBuffers(1, &bufferData.ebo);

  glBindVertexArray(bufferData.vao);

  // Bind the buffer object and set the vertex data
  glBindBuffer(GL_ARRAY_BUFFER, bufferData.vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(ZVertex), &vertices[0], GL_STATIC_DRAW);

  //  Bind the element buffer object (for indexed drawing) and set the index data
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferData.ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

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

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  return bufferData;
}

ZBufferData ZGLGraphicsStrategy::LoadVertexData(std::vector<float> vertices) {
  ZBufferData bufferData;
  glGenVertexArrays(1, &bufferData.vao);
  glGenBuffers(1, &bufferData.vbo);

  glBindVertexArray(bufferData.vao);

  // Bind the buffer object and set the vertex data
  glBindBuffer(GL_ARRAY_BUFFER, bufferData.vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

  // Vertex vector attribute
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  return bufferData;
}

ZTexture ZGLGraphicsStrategy::LoadDefaultTexture() {
  ZTexture texture;
  GLubyte textureData[] = { 255, 255, 255, 255 };
  glGenTextures(1, &texture.id);
  glBindTexture(GL_TEXTURE_2D, texture.id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
  return texture;
}

ZTexture ZGLGraphicsStrategy::LoadTexture(std::string path, const std::string &directory) {
  std::string filename = (!directory.empty() ? directory + '/' : "") + path;

  ZTexture texture;
  glGenTextures(1, &texture.id);

  int width, height, nrComponents;
  unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
  if (data) {
      glBindTexture(GL_TEXTURE_2D, texture.id);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  } else {
    ZLogger::Log("ZGLGraphicsStrategy Error: Failed to load texture at " + path, ZLoggerSeverity::Error);
  }

  stbi_image_free(data);

  return texture;
}

unsigned int ZGLGraphicsStrategy::LoadCubeMap(std::vector<std::string> faces) {
  unsigned int cubemapId;
  glGenTextures(1, &cubemapId);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapId);

  int width, height, nrChannels;
  for (unsigned int i = 0; i < faces.size(); i++) {
    unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
    if (data) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    } else {
      ZLogger::Log("Could not load texture at path " + faces[i], ZLoggerSeverity::Error);
    }
    stbi_image_free(data);
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  return cubemapId;
}

unsigned int ZGLGraphicsStrategy::LoadDepthMapBuffer(ZTexture depthTexture) {
  unsigned int depthFBO;
  glGenFramebuffers(1, &depthFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture.id, 0);
  glDrawBuffer(GL_NONE);

  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
    ZLogger::Log("Framebuffer operation incomplete dimensions", ZLoggerSeverity::Error);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return depthFBO;
}

ZTexture ZGLGraphicsStrategy::LoadDepthTexture() {
  ZTexture depthMap;
  depthMap.type = "depth";
  glGenTextures(1, &depthMap.id);
  glBindTexture(GL_TEXTURE_2D, depthMap.id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, ZEngine::SHADOW_MAP_SIZE, ZEngine::SHADOW_MAP_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  float borderColor[] = { 1.f, 1.f, 1.f, 1.f };
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  glBindTexture(GL_TEXTURE_2D, 0);
  return depthMap;
}

void ZGLGraphicsStrategy::BindDepthMapBuffer(unsigned int frameBuffer) {
  BindFramebuffer(frameBuffer);
  glViewport(0, 0, ZEngine::SHADOW_MAP_SIZE, ZEngine::SHADOW_MAP_SIZE);
  glClearDepth(1.0f);
  glClear(GL_DEPTH_BUFFER_BIT);

  glEnable(GL_POLYGON_OFFSET_FILL); // Prevents Z-Fighting
  glPolygonOffset(2.f, 4.f);
  glCullFace(GL_FRONT); // Prevents peter-panning
}

void ZGLGraphicsStrategy::UnbindDepthMapBuffer() {
  glCullFace(GL_BACK);
  glDisable(GL_POLYGON_OFFSET_FILL);

  UnbindFramebuffer();
  glViewport(0, 0, ZEngine::Domain()->WindowWidth() * 2, ZEngine::Domain()->WindowHeight() * 2);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ZGLGraphicsStrategy::Draw(ZBufferData bufferData, std::vector<ZVertex> vertices, std::vector<unsigned int> indices) {
  glBindVertexArray(bufferData.vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferData.ebo);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  glActiveTexture(GL_TEXTURE0);
}

void ZGLGraphicsStrategy::Draw(ZBufferData bufferData, std::vector<float> vertices) {
  glBindVertexArray(bufferData.vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size());
  glBindVertexArray(0);
  glActiveTexture(GL_TEXTURE0);
}

void ZGLGraphicsStrategy::CleanUp() {

}

void ZGLGraphicsStrategy::GLFWErrorCallback(int id, const char* description) {
  ZLogger::Log(description, ZLoggerSeverity::Error);
}
