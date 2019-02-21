//
//  TModel.hpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZCommon.hpp"
#include "ZEngine.hpp"
#include "ZMesh3D.hpp"

// Forward Declarations
class ZShader;

// Class Definitions
class ZModel {
private:

  std::vector<ZMesh3D> meshes_;

  void CreatePlane(glm::vec3 scale, std::vector<ZTexture> textures);
  void CreateCube(glm::vec3 scale, std::vector<ZTexture> textures);
  void CreateSphere(glm::vec3 scale, std::vector<ZTexture> textures);
  void CreateCylinder(glm::vec3 scale, std::vector<ZTexture> textures);
  void CreateCone(glm::vec3 scale, std::vector<ZTexture> textures);

public:

  ZModel(ZPrimitiveType primitiveType, glm::vec3 scale = glm::vec3(0.5f, 0.f, 0.5f), std::vector<ZTexture> textures = {});
  ZModel(std::string path);
  ZModel() { }
  ~ZModel() { }

  void SetMaterial(ZMaterial material);

  virtual void Render(ZShader* shader);

  static ZModel* NewPlanePrimitive(glm::vec3 scale = glm::vec3(0.5f, 0.f, 0.5f), std::vector<ZTexture> textures = {});
  static ZModel* NewCubePrimitive(glm::vec3 scale = glm::vec3(0.5f, 0.5f, 0.5f), std::vector<ZTexture> textures = {});
  static ZModel* NewSpherePrimitive(glm::vec3 scale = glm::vec3(0.5f, 0.5f, 0.5f), std::vector<ZTexture> textures = {});
  static ZModel* NewCylinderPrimitive(glm::vec3 scale = glm::vec3(0.5f, 0.5f, 0.5f), std::vector<ZTexture> textures = {});
  static ZModel* NewConePrimitive(glm::vec3 scale = glm::vec3(0.5f, 0.5f, 0.5f), std::vector<ZTexture> textures = {});
  static ZModel* NewSkybox(std::vector<std::string> faces = ZEngine::DEFAULT_SKYBOX_CUBEMAP);
};
