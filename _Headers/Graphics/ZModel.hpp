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

  ZMesh3DMap meshes_;

  void CreatePlane(glm::vec3 scale, std::vector<ZTexture> textures);
  void CreateCube(glm::vec3 scale, std::vector<ZTexture> textures);
  void CreateSphere(glm::vec3 scale, std::vector<ZTexture> textures);
  void CreateCylinder(glm::vec3 scale, std::vector<ZTexture> textures);
  void CreateCone(glm::vec3 scale, std::vector<ZTexture> textures);

public:

  ZModel(ZPrimitiveType primitiveType, glm::vec3 scale = glm::vec3(1.0f, 0.f, 1.0f), std::vector<ZTexture> textures = {});
  ZModel(std::string path);
  ZModel() { }
  virtual ~ZModel() { }

  virtual void Render(ZShader* shader, std::vector<std::shared_ptr<ZMaterial>> materials);

  const ZMesh3DMap& Meshes() { return meshes_; }

  static std::unique_ptr<ZModel> NewPlanePrimitive(glm::vec3 scale = glm::vec3(1.0f, 0.f, 1.0f), std::vector<ZTexture> textures = {});
  static std::unique_ptr<ZModel> NewCubePrimitive(glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), std::vector<ZTexture> textures = {});
  static std::unique_ptr<ZModel> NewSpherePrimitive(glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), std::vector<ZTexture> textures = {});
  static std::unique_ptr<ZModel> NewCylinderPrimitive(glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), std::vector<ZTexture> textures = {});
  static std::unique_ptr<ZModel> NewConePrimitive(glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), std::vector<ZTexture> textures = {});
  static std::unique_ptr<ZModel> NewSkybox(std::vector<std::string> faces = ZEngine::DEFAULT_SKYBOX_CUBEMAP);
  static std::unique_ptr<ZModel> NewSkybox(std::string equirectHDR, ZIBLTexture& generatedIBLTexture);
};
