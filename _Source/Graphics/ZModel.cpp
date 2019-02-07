//
//  ZModel.cpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZModel.hpp"
#include "ZShader.hpp"
#include "ZGLMesh3D.hpp"
#include "ZGLModelImporter.hpp"
#include "ZLogger.hpp"

#include "stb_image.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

ZModel::ZModel(ZPrimitiveType primitiveType, glm::vec3 scale, std::vector<ZTexture> textures) {
  switch (primitiveType) {
    case ZPrimitiveType::Plane:
      CreatePlane(scale, textures); break;
    case ZPrimitiveType::Cube:
      CreateCube(scale, textures); break;
    case ZPrimitiveType::Sphere:
      CreateSphere(scale, textures); break;
    case ZPrimitiveType::Cylinder:
      CreateCylinder(scale, textures); break;
    case ZPrimitiveType::Cone:
      CreateCone(scale, textures); break;
  }
}

ZModel::ZModel(std::string path) {
  ZGLModelImporter importer;
  importer.LoadModel(path, meshes_);
}

void ZModel::Render(ZShader* shader) {
    for (unsigned int i = 0; i < meshes_.size(); i++) {
        meshes_[i].Render(shader);
    }
}

void ZModel::SetMaterial(ZMaterial material) {
  for (unsigned int i = 0; i < meshes_.size(); i++) {
      meshes_[i].SetMaterial(material);
  }
}

/**
 *  Plane Creation
 */
ZModel* ZModel::NewPlanePrimitive(glm::vec3 scale, std::vector<ZTexture> textures) {
  return new ZModel(ZPrimitiveType::Plane, scale, textures);
}

void ZModel::CreatePlane(glm::vec3 scale, std::vector<ZTexture> textures) {
  ZVertex topRight(glm::vec3(scale.x, 0.0f, -scale.z));
  ZVertex bottomRight(glm::vec3(scale.x, 0.0f, scale.z));
  ZVertex bottomLeft(glm::vec3(-scale.x, 0.0f, scale.z));
  ZVertex topLeft(glm::vec3(-scale.x, 0.0f, -scale.z));

  std::vector<ZVertex> vertices = {
    topRight, bottomRight, bottomLeft, topLeft
  };

  std::vector<unsigned int> indices = {
    0, 1, 3,
    1, 2, 3
  };

  ZMaterial material = textures.size() > 0 ? ZMaterial(textures) : ZMaterial::DefaultMaterial();

  meshes_.push_back(ZGLMesh3D(vertices, indices, material));
}

/**
 *  Cube Creation
 */
ZModel* ZModel::NewCubePrimitive(glm::vec3 scale, std::vector<ZTexture> textures) {
  return new ZModel(ZPrimitiveType::Cube, scale, textures);
}

void ZModel::CreateCube(glm::vec3 scale, std::vector<ZTexture> textures) {
  // Front face
  ZVertex front_BottomLeft(glm::vec3(-scale.x, -scale.y, scale.z), glm::vec3(0.f, 0.f, 1.f));
  ZVertex front_BottomRight(glm::vec3(scale.x, -scale.y, scale.z), glm::vec3(0.f, 0.f, 1.f));
  ZVertex front_TopRight(glm::vec3(scale.x, scale.y, scale.z), glm::vec3(0.f, 0.f, 1.f));
  ZVertex front_TopLeft(glm::vec3(-scale.x, scale.y, scale.z), glm::vec3(0.f, 0.f, 1.f));
  // Back face
  ZVertex back_BottomLeft(glm::vec3(scale.x, -scale.y, -scale.z), glm::vec3(0.f, 0.f, -1.f));
  ZVertex back_BottomRight(glm::vec3(-scale.x, -scale.y, -scale.z), glm::vec3(0.f, 0.f, -1.f));
  ZVertex back_TopRight(glm::vec3(-scale.x, scale.y, -scale.z), glm::vec3(0.f, 0.f, -1.f));
  ZVertex back_TopLeft(glm::vec3(scale.x, scale.y, -scale.z), glm::vec3(0.f, 0.f, -1.f));
  // Right Face
  ZVertex right_BottomLeft(glm::vec3(scale.x, -scale.y, scale.z), glm::vec3(1.f, 0.f, 0.f));
  ZVertex right_BottomRight(glm::vec3(scale.x, -scale.y, -scale.z), glm::vec3(1.f, 0.f, 0.f));
  ZVertex right_TopRight(glm::vec3(scale.x, scale.y, -scale.z), glm::vec3(1.f, 0.f, 0.f));
  ZVertex right_TopLeft(glm::vec3(scale.x, scale.y, scale.z), glm::vec3(1.f, 0.f, 0.f));
  // Left face
  ZVertex left_BottomLeft(glm::vec3(-scale.x, -scale.y, -scale.z), glm::vec3(-1.f, 0.f, 0.f));
  ZVertex left_BottomRight(glm::vec3(-scale.x, -scale.y, scale.z), glm::vec3(-1.f, 0.f, 0.f));
  ZVertex left_TopRight(glm::vec3(-scale.x, scale.y, scale.z), glm::vec3(-1.f, 0.f, 0.f));
  ZVertex left_TopLeft(glm::vec3(-scale.x, scale.y, -scale.z), glm::vec3(-1.f, 0.f, 0.f));
  // Top face
  ZVertex top_BottomLeft(glm::vec3(-scale.x, scale.y, scale.z), glm::vec3(0.f, 1.f, 0.f));
  ZVertex top_BottomRight(glm::vec3(scale.x, scale.y, scale.z), glm::vec3(0.f, 1.f, 0.f));
  ZVertex top_TopRight(glm::vec3(scale.x, scale.y, -scale.z), glm::vec3(0.f, 1.f, 0.f));
  ZVertex top_TopLeft(glm::vec3(-scale.x, scale.y, -scale.z), glm::vec3(0.f, 1.f, 0.f));
  // Bottom face
  ZVertex bottom_BottomLeft(glm::vec3(-scale.x, -scale.y, scale.z), glm::vec3(0.f, -1.f, 0.f));
  ZVertex bottom_BottomRight(glm::vec3(scale.x, -scale.y, scale.z), glm::vec3(0.f, -1.f, 0.f));
  ZVertex bottom_TopRight(glm::vec3(scale.x, -scale.y, -scale.z), glm::vec3(0.f, -1.f, 0.f));
  ZVertex bottom_TopLeft(glm::vec3(-scale.x, -scale.y, -scale.z), glm::vec3(0.f, -1.f, 0.f));

  std::vector<ZVertex> vertices = {
    front_BottomLeft, front_BottomRight, front_TopRight, front_TopLeft,
    right_BottomLeft, right_BottomRight, right_TopRight, right_TopLeft,
    back_BottomLeft, back_BottomRight, back_TopRight, back_TopLeft,
    left_BottomLeft, left_BottomRight, left_TopRight, left_TopLeft,
    top_BottomLeft, top_BottomRight, top_TopRight, top_TopLeft,
    bottom_BottomLeft, bottom_BottomRight, bottom_TopRight, bottom_TopLeft
  };

  std::vector<unsigned int> indices = {
    0, 1, 2, 0, 2, 3,
    4, 5, 6, 4, 6, 7,
    8, 9, 10, 8, 10, 11,
    12, 13, 14, 12, 14, 15,
    16, 17, 18, 16, 18, 19,
    20, 21, 22, 20, 22, 23
  };

  ZMaterial material = textures.size() > 0 ? ZMaterial(textures) : ZMaterial::DefaultMaterial();

  meshes_.push_back(ZGLMesh3D(vertices, indices, material));
}

/**
 *  Sphere Creation
 */
ZModel* ZModel::NewSpherePrimitive(glm::vec3 scale, std::vector<ZTexture> textures) {
  return new ZModel(ZPrimitiveType::Sphere, scale, textures);
}

void ZModel::CreateSphere(glm::vec3 scale, std::vector<ZTexture> textures) {

}

/**
 *  Cylinder Creation
 */
ZModel* ZModel::NewCylinderPrimitive(glm::vec3 scale, std::vector<ZTexture> textures) {
  return new ZModel(ZPrimitiveType::Cylinder, scale, textures);
}

void ZModel::CreateCylinder(glm::vec3 scale, std::vector<ZTexture> textures) {

}

/**
 *  Cone Creation
 */
ZModel* ZModel::NewConePrimitive(glm::vec3 scale, std::vector<ZTexture> textures) {
  return new ZModel(ZPrimitiveType::Cone, scale, textures);
}

void ZModel::CreateCone(glm::vec3 scale, std::vector<ZTexture> textures) {

}

/**
 *  Skybox Creation
 */
ZModel* ZModel::NewSkybox(std::vector<std::string> faces) {
  ZModel* skybox;

  // TODO: Extract this and place it in the ZGLGraphics implementation since it is OGL specific
  /////////////////////////////////////////////////////////////////////////////////////////////
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
  /////////////////////////////////////////////////////////////////////////////////////////////

  std::vector<ZTexture> textures;
  for (std::string facePath : faces) {
    ZTexture texture;
    texture.id = cubemapId;
    texture.type = "cubemap";
    texture.path = facePath;
    textures.push_back(texture);
  }

  skybox = NewCubePrimitive(glm::vec3(1.f, 1.f, 1.f), textures);

  return skybox;
}
