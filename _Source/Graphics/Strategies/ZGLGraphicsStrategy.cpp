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
#include "ZModel.hpp"
#include "ZShader.hpp"
#include "ZCommon.hpp"

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

void ZGLGraphicsStrategy::DisableDepthTesting() {
  glDisable(GL_DEPTH_TEST);
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

void ZGLGraphicsStrategy::EnableFaceCulling() {
  glEnable(GL_CULL_FACE);
}

void ZGLGraphicsStrategy::DisableFaceCulling() {
  glDisable(GL_CULL_FACE);
}

void ZGLGraphicsStrategy::BindFramebuffer(ZBufferData frameBuffer) {
  glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.fbo);
}

void ZGLGraphicsStrategy::UnbindFramebuffer() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ZGLGraphicsStrategy::BindTexture(ZTexture texture, unsigned int index) {
  glActiveTexture(GL_TEXTURE0 + index);
  if (texture.type == "cubemap" || texture.type == "irradiance") {
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture.id);
  } else {
    glBindTexture(GL_TEXTURE_2D, texture.id);
  }
}

ZBufferData ZGLGraphicsStrategy::LoadIndexedVertexData(std::vector<ZVertex3D> vertices, std::vector<unsigned int> indices) {
  ZBufferData bufferData;
  glGenVertexArrays(1, &bufferData.vao);
  glGenBuffers(1, &bufferData.vbo);
  glGenBuffers(1, &bufferData.ebo);

  glBindVertexArray(bufferData.vao);

  // Bind the buffer object and set the vertex data
  glBindBuffer(GL_ARRAY_BUFFER, bufferData.vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(ZVertex3D), &vertices[0], GL_STATIC_DRAW);

  //  Bind the element buffer object (for indexed drawing) and set the index data
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferData.ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

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

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  return bufferData;
}

ZBufferData ZGLGraphicsStrategy::LoadVertexData(std::vector<ZVertex3D> vertices) {
  ZBufferData bufferData;
  glGenVertexArrays(1, &bufferData.vao);
  glGenBuffers(1, &bufferData.vbo);

  glBindVertexArray(bufferData.vao);

  // Bind the buffer object and set the vertex data
  glBindBuffer(GL_ARRAY_BUFFER, bufferData.vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(ZVertex3D), &vertices[0], GL_STATIC_DRAW);

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

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  return bufferData;
}

ZBufferData ZGLGraphicsStrategy::LoadVertexData(std::vector<ZVertex2D> vertices) {
  ZBufferData bufferData;
  glGenVertexArrays(1, &bufferData.vao);
  glGenBuffers(1, &bufferData.vbo);

  glBindVertexArray(bufferData.vao);

  // Bind the buffer object and set the vertex data
  glBindBuffer(GL_ARRAY_BUFFER, bufferData.vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(ZVertex2D), &vertices[0], GL_STATIC_DRAW);

  // Vertex position attribute
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ZVertex2D), (void*)0);
  glEnableVertexAttribArray(0);

  // Vertex texture coordinates attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ZVertex2D), (void*)offsetof(ZVertex2D, uv));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  return bufferData;
}

ZBufferData ZGLGraphicsStrategy::LoadEmptyVertexData2D(unsigned int size) {
  ZBufferData bufferData;
  glGenVertexArrays(1, &bufferData.vao);
  glGenBuffers(1, &bufferData.vbo);

  glBindVertexArray(bufferData.vao);

  // Bind the buffer object and set the vertex data
  glBindBuffer(GL_ARRAY_BUFFER, bufferData.vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(ZVertex2D) * size, NULL, GL_STATIC_DRAW);

  // Vertex position attribute
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ZVertex2D), (void*)0);
  glEnableVertexAttribArray(0);

  // Vertex texture coordinates attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ZVertex2D), (void*)offsetof(ZVertex2D, uv));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  return bufferData;
}

void ZGLGraphicsStrategy::DeleteBufferData(ZBufferData bufferData) {
  glDeleteVertexArrays(1, &bufferData.vao);
  glDeleteBuffers(1, &bufferData.vbo);
  glDeleteBuffers(1, &bufferData.ebo);
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

ZTexture ZGLGraphicsStrategy::LoadTexture(std::string path, const std::string &directory, bool hdr, bool flip) {
  std::string filename = (!directory.empty() ? directory + '/' : "") + path;

  ZTexture texture;
  glGenTextures(1, &texture.id);
  glBindTexture(GL_TEXTURE_2D, texture.id);

  if (flip) { stbi_set_flip_vertically_on_load(true); }

  int width, height, nrComponents;
  if (hdr) {
  float *data = stbi_loadf(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
      _Z("ZGLGraphicsStrategy Error: Failed to load texture at " + path, ZERROR);
    }
    stbi_image_free(data);
  } else {
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
      _Z("ZGLGraphicsStrategy Error: Failed to load texture at " + path, ZERROR);
    }
    stbi_image_free(data);
  }

  return texture;
}

ZTexture ZGLGraphicsStrategy::LoadCubeMap(std::vector<std::string> faces) {
  ZTexture cubemap;
  cubemap.type = "cubemap";
  glGenTextures(1, &cubemap.id);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.id);

  int width, height, nrChannels;
  for (unsigned int i = 0; i < faces.size(); i++) {
    unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
    if (data) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    } else {
      _Z("Could not load texture at path " + faces[i], ZERROR);
    }
    stbi_image_free(data);
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  return cubemap;
}

ZTexture ZGLGraphicsStrategy::LoadEmptyCubeMap(bool irradiance) {
  ZTexture cubemap;
  cubemap.type = "cubemap";
  glGenTextures(1, &cubemap.id);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.id);

  for (unsigned int i = 0; i < 6; i++) {
    unsigned int size = irradiance ? ZEngine::IRRADIANCE_MAP_SIZE : ZEngine::CUBE_MAP_SIZE;
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, size, size, 0, GL_RGB, GL_FLOAT, nullptr);
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  return cubemap;
}

ZBufferData ZGLGraphicsStrategy::LoadDepthMapBuffer(ZTexture depthTexture) {
  ZBufferData depth;
  depth.type = ZBufferDataType::FrameBuffer;
  glGenFramebuffers(1, &depth.fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, depth.fbo);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture.id, 0);
  glDrawBuffer(GL_NONE);

  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
    _Z("Framebuffer operation incomplete dimensions", ZERROR);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return depth;
}

ZBufferData ZGLGraphicsStrategy::LoadCubeMapBuffer() {
  ZBufferData capture;
  glGenFramebuffers(1, &capture.fbo);
  glGenRenderbuffers(1, &capture.rbo);

  glBindFramebuffer(GL_FRAMEBUFFER, capture.fbo);
  glBindRenderbuffer(GL_RENDERBUFFER, capture.rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, ZEngine::CUBE_MAP_SIZE, ZEngine::CUBE_MAP_SIZE);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, capture.rbo);
  return capture;
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

void ZGLGraphicsStrategy::BindDepthMapBuffer(ZBufferData frameBuffer) {
  BindFramebuffer(frameBuffer);
  glViewport(0, 0, ZEngine::SHADOW_MAP_SIZE, ZEngine::SHADOW_MAP_SIZE);
  glClearDepth(1.0f);
  glClear(GL_DEPTH_BUFFER_BIT);
}

void ZGLGraphicsStrategy::UnbindDepthMapBuffer() {
  UnbindFramebuffer();
  glViewport(0, 0, ZEngine::Domain()->ResolutionX(), ZEngine::Domain()->ResolutionY());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ZGLGraphicsStrategy::BindCubeMapBuffer(ZBufferData cubemapBuffer) {
  glViewport(0, 0, ZEngine::CUBE_MAP_SIZE, ZEngine::CUBE_MAP_SIZE);
  BindFramebuffer(cubemapBuffer);
}

void ZGLGraphicsStrategy::UpdateBuffer(ZBufferData buffer, std::vector<ZVertex2D> data) {
  glBindVertexArray(buffer.vao);
  glBindBuffer(GL_ARRAY_BUFFER, buffer.vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, data.size() * sizeof(ZVertex2D), &data[0]);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

ZTexture ZGLGraphicsStrategy::EquirectToCubemap(std::string equirectHDRPath, ZBufferData& bufferData) {
  bufferData = LoadCubeMapBuffer();
  ZTexture cubeMap = LoadEmptyCubeMap();
  ZTexture hdrTexture = LoadTexture(equirectHDRPath, "", true, true);

  glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.f, 0.1f, 100.0f);
  glm::mat4 captureViews[] = {
    glm::lookAt(glm::vec3(0.0f), glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f)),
    glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f)),
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 1.f)),
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.f, 0.f, -1.f)),
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, -1.f, 0.f)),
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, -1.f, 0.f)),
  };

  ZModel* cube = ZModel::NewCubePrimitive(glm::vec3(1.f, 1.f, 1.f));
  ZShader equirectToCubemapShader;
  equirectToCubemapShader.Initialize("Assets/Shaders/Vertex/basic.vert", "Assets/Shaders/Pixel/equirect_to_cube.frag");
  equirectToCubemapShader.Activate();
  equirectToCubemapShader.SetInt("equirectangularMap", 0);
  equirectToCubemapShader.SetMat4("P", captureProjection);

  glViewport(0, 0, ZEngine::CUBE_MAP_SIZE, ZEngine::CUBE_MAP_SIZE);
  BindCubeMapBuffer(bufferData);
  for (unsigned int i = 0; i < 6; i++) {
    equirectToCubemapShader.SetMat4("V", captureViews[i]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeMap.id, 0);
    ClearViewport();

    cube->Render(&equirectToCubemapShader);
  }
  UnbindFramebuffer();

  delete cube;

  return cubeMap;
}

ZTexture ZGLGraphicsStrategy::IrradianceMapFromCubeMap(ZBufferData cubemapBufferData, ZTexture cubemapTexture) {
  ZTexture irradianceMap = LoadEmptyCubeMap(true);
  irradianceMap.type = "irradiance";
  glBindFramebuffer(GL_FRAMEBUFFER, cubemapBufferData.fbo);
  glBindRenderbuffer(GL_RENDERBUFFER, cubemapBufferData.rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, ZEngine::IRRADIANCE_MAP_SIZE, ZEngine::IRRADIANCE_MAP_SIZE);

  glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.f, 0.1f, 100.0f);
  glm::mat4 captureViews[] = {
    glm::lookAt(glm::vec3(0.0f), glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f)),
    glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f)),
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 1.f)),
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.f, 0.f, -1.f)),
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, -1.f, 0.f)),
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, -1.f, 0.f)),
  };

  ZModel* cube = ZModel::NewCubePrimitive(glm::vec3(1.f, 1.f, 1.f));
  ZShader irradianceShader;
  irradianceShader.Initialize("Assets/Shaders/Vertex/basic.vert", "Assets/Shaders/Pixel/irradiance.frag");
  irradianceShader.Activate();
  irradianceShader.SetInt("environmentMap", 0);
  irradianceShader.SetMat4("P", captureProjection);
  BindTexture(cubemapTexture, 0);
  BindCubeMapBuffer(cubemapBufferData);
  for (unsigned int i = 0; i < 6; i++) {
    irradianceShader.SetMat4("V", captureViews[i]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap.id, 0);
    ClearViewport();

    cube->Render(&irradianceShader);
  }
  UnbindFramebuffer();

  delete cube;

  return irradianceMap;
}

void ZGLGraphicsStrategy::Draw(ZBufferData bufferData, std::vector<ZVertex3D> vertices, std::vector<unsigned int> indices) {
  glBindVertexArray(bufferData.vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferData.ebo);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  glActiveTexture(GL_TEXTURE0);
}

void ZGLGraphicsStrategy::Draw(ZBufferData bufferData, std::vector<ZVertex2D> vertices) {
  glBindVertexArray(bufferData.vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size());
  glBindVertexArray(0);
  glActiveTexture(GL_TEXTURE0);
}

void ZGLGraphicsStrategy::DrawLines(ZBufferData bufferData, std::vector<ZVertex3D> vertices) {
   glBindVertexArray(bufferData.vao);
   glDrawArrays(GL_LINES, 0, 2);
   glBindVertexArray(0);
   glActiveTexture(GL_TEXTURE0);
}

void ZGLGraphicsStrategy::GLFWErrorCallback(int id, const char* description) {
  _Z(description, ZERROR);
}
