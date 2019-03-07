//
//  ZModel.cpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZEngine.hpp"
#include "ZModel.hpp"
#include "ZShader.hpp"
#include "ZMesh3D.hpp"
#include "ZGLModelImporter.hpp"
#include "ZCommon.hpp"

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

void ZModel::Render(ZShader* shader, std::vector<std::shared_ptr<ZMaterial>> materials) {
  std::vector<std::shared_ptr<ZMaterial>> meshesLeft = meshes_;
  for (auto it = materialsLeft.begin(); it != materialsLeft.end(); it++) {
    if (!(*it)->MeshID().empty()) {
      meshes_[(*it)->MeshID()]->Render(*it);
      meshesLeft.erase((*it)->MeshID());
    } else {
      for (ZMesh3DMap::iterator it = meshesLeft.begin(); it != meshesLeft.end(); it++) {
          it->second->Render(shader, material);
      }
    }
  }
}

/**
 *  Plane Creation
 */
std::unique_ptr<ZModel> ZModel::NewPlanePrimitive(glm::vec3 scale, std::vector<ZTexture> textures) {
  return std::unique_ptr<ZModel>(new ZModel(ZPrimitiveType::Plane, scale, textures));
}

void ZModel::CreatePlane(glm::vec3 scale, std::vector<ZTexture> textures) {
  ZVertex3D topRight(glm::vec3(scale.x, 0.0f, -scale.z)); topRight.uv = glm::vec2(1.f, 1.f);
  ZVertex3D bottomRight(glm::vec3(scale.x, 0.0f, scale.z)); bottomRight.uv = glm::vec2(1.f, 0.f);
  ZVertex3D bottomLeft(glm::vec3(-scale.x, 0.0f, scale.z)); bottomLeft.uv = glm::vec2(0.f, 0.f);
  ZVertex3D topLeft(glm::vec3(-scale.x, 0.0f, -scale.z)); topLeft.uv = glm::vec2(0.f, 1.f);

  std::vector<ZVertex3D> vertices = {
    topRight, bottomRight, bottomLeft, topLeft
  };

  std::vector<unsigned int> indices = {
    0, 1, 3,
    1, 2, 3
  };

  std::shared_ptr<ZMesh3D> mesh = std::make_shared<ZMesh3D>(vertices, indices);
  meshes_[mesh->ID()] = mesh;
}

/**
 *  Cube Creation
 */
std::unique_ptr<ZModel> ZModel::NewCubePrimitive(glm::vec3 scale, std::vector<ZTexture> textures) {
  return std::unique_ptr<ZModel>(new ZModel(ZPrimitiveType::Cube, scale, textures));
}

void ZModel::CreateCube(glm::vec3 scale, std::vector<ZTexture> textures) {
  // Front face
  ZVertex3D front_BottomLeft(glm::vec3(-scale.x, -scale.y, scale.z), glm::vec3(0.f, 0.f, 1.f));
  ZVertex3D front_BottomRight(glm::vec3(scale.x, -scale.y, scale.z), glm::vec3(0.f, 0.f, 1.f));
  ZVertex3D front_TopRight(glm::vec3(scale.x, scale.y, scale.z), glm::vec3(0.f, 0.f, 1.f));
  ZVertex3D front_TopLeft(glm::vec3(-scale.x, scale.y, scale.z), glm::vec3(0.f, 0.f, 1.f));
  // Back face
  ZVertex3D back_BottomLeft(glm::vec3(scale.x, -scale.y, -scale.z), glm::vec3(0.f, 0.f, -1.f));
  ZVertex3D back_BottomRight(glm::vec3(-scale.x, -scale.y, -scale.z), glm::vec3(0.f, 0.f, -1.f));
  ZVertex3D back_TopRight(glm::vec3(-scale.x, scale.y, -scale.z), glm::vec3(0.f, 0.f, -1.f));
  ZVertex3D back_TopLeft(glm::vec3(scale.x, scale.y, -scale.z), glm::vec3(0.f, 0.f, -1.f));
  // Right Face
  ZVertex3D right_BottomLeft(glm::vec3(scale.x, -scale.y, scale.z), glm::vec3(1.f, 0.f, 0.f));
  ZVertex3D right_BottomRight(glm::vec3(scale.x, -scale.y, -scale.z), glm::vec3(1.f, 0.f, 0.f));
  ZVertex3D right_TopRight(glm::vec3(scale.x, scale.y, -scale.z), glm::vec3(1.f, 0.f, 0.f));
  ZVertex3D right_TopLeft(glm::vec3(scale.x, scale.y, scale.z), glm::vec3(1.f, 0.f, 0.f));
  // Left face
  ZVertex3D left_BottomLeft(glm::vec3(-scale.x, -scale.y, -scale.z), glm::vec3(-1.f, 0.f, 0.f));
  ZVertex3D left_BottomRight(glm::vec3(-scale.x, -scale.y, scale.z), glm::vec3(-1.f, 0.f, 0.f));
  ZVertex3D left_TopRight(glm::vec3(-scale.x, scale.y, scale.z), glm::vec3(-1.f, 0.f, 0.f));
  ZVertex3D left_TopLeft(glm::vec3(-scale.x, scale.y, -scale.z), glm::vec3(-1.f, 0.f, 0.f));
  // Top face
  ZVertex3D top_BottomLeft(glm::vec3(-scale.x, scale.y, scale.z), glm::vec3(0.f, 1.f, 0.f));
  ZVertex3D top_BottomRight(glm::vec3(scale.x, scale.y, scale.z), glm::vec3(0.f, 1.f, 0.f));
  ZVertex3D top_TopRight(glm::vec3(scale.x, scale.y, -scale.z), glm::vec3(0.f, 1.f, 0.f));
  ZVertex3D top_TopLeft(glm::vec3(-scale.x, scale.y, -scale.z), glm::vec3(0.f, 1.f, 0.f));
  // Bottom face
  ZVertex3D bottom_BottomLeft(glm::vec3(-scale.x, -scale.y, scale.z), glm::vec3(0.f, -1.f, 0.f));
  ZVertex3D bottom_BottomRight(glm::vec3(scale.x, -scale.y, scale.z), glm::vec3(0.f, -1.f, 0.f));
  ZVertex3D bottom_TopRight(glm::vec3(scale.x, -scale.y, -scale.z), glm::vec3(0.f, -1.f, 0.f));
  ZVertex3D bottom_TopLeft(glm::vec3(-scale.x, -scale.y, -scale.z), glm::vec3(0.f, -1.f, 0.f));

  std::vector<ZVertex3D> vertices = {
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

  std::shared_ptr<ZMesh3D> mesh = std::make_shared<ZMesh3D>(vertices, indices);
  meshes_[mesh->ID()] = mesh;
}

/**
 *  Sphere Creation
 */
std::unique_ptr<ZModel> ZModel::NewSpherePrimitive(glm::vec3 scale, std::vector<ZTexture> textures) {
  return std::unique_ptr<ZModel>(new ZModel(ZPrimitiveType::Sphere, scale, textures));
}

void ZModel::CreateSphere(glm::vec3 scale, std::vector<ZTexture> textures) {
  std::vector<ZVertex3D> vertices;
  std::vector<unsigned int> indices;

  const unsigned int X_SEGMENTS = 64;
  const unsigned int Y_SEGMENTS = 64;
  for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
    for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
      float xSegment = (float)x / (float)X_SEGMENTS;
      float ySegment = (float)y / (float)Y_SEGMENTS;
      float xPos = std::cos(xSegment * 2.0f * glm::pi<float>()) * std::sin(ySegment * glm::pi<float>());
      float yPos = std::cos(ySegment * glm::pi<float>());
      float zPos = std::sin(xSegment * 2.0f * glm::pi<float>()) * std::sin(ySegment * glm::pi<float>());

      ZVertex3D vertex(glm::vec3(xPos, yPos, zPos), glm::vec3(xPos, yPos, zPos));
      vertex.uv = glm::vec2(xSegment, ySegment);

      vertices.push_back(vertex);
    }
  }

  bool oddRow = false;
  for (int y = 0; y < Y_SEGMENTS; ++y) {
    if (!oddRow) {
      for (int x = 0; x <= X_SEGMENTS; ++x) {
        indices.push_back(y * (X_SEGMENTS + 1) + x);
        indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
      }
    } else {
      for (int x = X_SEGMENTS; x >= 0; --x) {
        indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
        indices.push_back(y * (X_SEGMENTS + 1) + x);
      }
    }
    oddRow = !oddRow;
  }

  std::shared_ptr<ZMesh3D> mesh = std::make_shared<ZMesh3D>(vertices, indices, ZMeshDrawStyle::TriangleStrip);
  meshes_[mesh->ID()] = mesh;
}

/**
 *  Cylinder Creation
 */
std::unique_ptr<ZModel> ZModel::NewCylinderPrimitive(glm::vec3 scale, std::vector<ZTexture> textures) {
  return std::unique_ptr<ZModel>(new ZModel(ZPrimitiveType::Cylinder, scale, textures));
}

void ZModel::CreateCylinder(glm::vec3 scale, std::vector<ZTexture> textures) {

}

/**
 *  Cone Creation
 */
std::unique_ptr<ZModel> ZModel::NewConePrimitive(glm::vec3 scale, std::vector<ZTexture> textures) {
  return std::unique_ptr<ZModel>(new ZModel(ZPrimitiveType::Cone, scale, textures));
}

void ZModel::CreateCone(glm::vec3 scale, std::vector<ZTexture> textures) {

}

/**
 *  Skybox Creation
 */
std::unique_ptr<ZModel> ZModel::NewSkybox(std::vector<std::string> faces) {
  ZTexture cubeMap = ZEngine::Graphics()->Strategy()->LoadCubeMap(faces);
  std::vector<ZTexture> textures = { cubeMap };

  return NewCubePrimitive(glm::vec3(1.f, 1.f, 1.f), textures);
}

std::unique_ptr<ZModel> ZModel::NewSkybox(std::string equirectHDR, ZIBLTexture& generatedIBLTexture) {
  ZBufferData cubemapBuffer;
  ZTexture cubeMap = ZEngine::Graphics()->Strategy()->EquirectToCubemap(equirectHDR, cubemapBuffer);

  generatedIBLTexture.irradiance = ZEngine::Graphics()->Strategy()->IrradianceMapFromCubeMap(cubemapBuffer, cubeMap);
  generatedIBLTexture.prefiltered = ZEngine::Graphics()->Strategy()->PrefilterCubeMap(cubemapBuffer, cubeMap);
  generatedIBLTexture.brdfLUT = ZEngine::Graphics()->Strategy()->BRDFLUT(cubemapBuffer);

  std::vector<ZTexture> textures = { cubeMap };

  return NewCubePrimitive(glm::vec3(1.f, 1.f, 1.f), textures);
}
