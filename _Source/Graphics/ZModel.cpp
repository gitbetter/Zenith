//
//  ZModel.cpp
//  Starter
//
//  Created by Adrian Sanchez on 1/25/19.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZModel.hpp"
#include "ZShader.hpp"
#include "ZModelImporter.hpp"
#include <glm/glm.hpp>

ZModel::ZModel(ZPrimitiveType primitiveType, glm::vec3 scale) {
  switch (primitiveType) {
    case ZPrimitiveType::Plane:
      CreatePlane(scale); break;
    case ZPrimitiveType::Cube:
      CreateCube(scale); break;
    case ZPrimitiveType::Sphere:
      CreateSphere(scale); break;
    case ZPrimitiveType::Cylinder:
      CreateCylinder(scale); break;
    case ZPrimitiveType::Cone:
      CreateCube(scale); break;
  }
}

ZModel::ZModel(std::string path) {
  ZModelImporter importer;
  importer.LoadModel(path, meshes_);
}

void ZModel::Render(ZShader* shader) {
    for (unsigned int i = 0; i < meshes_.size(); i++) {
        meshes_[i].Render(shader);
    }
}

/**
 *  Plane Creation
 */
ZModel ZModel::NewPlanePrimitive(glm::vec3 scale) {
  return ZModel(ZPrimitiveType::Plane, scale);
}

void ZModel::CreatePlane(glm::vec3 scale) {
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

  ZMaterialProperties materialProperties;
  materialProperties.albedo = glm::vec4(0.3f, 0.3f, 0.3f, 1.f);
  materialProperties.diffuse = glm::vec3(0.3f);
  materialProperties.ambient = glm::vec3(0.2f);
  materialProperties.specular = glm::vec3(0.7f);
  materialProperties.shininess = 5.f;
  ZMaterial material(materialProperties);

  meshes_.push_back(ZMesh(vertices, indices, material));
}

/**
 *  Cube Creation
 */
ZModel ZModel::NewCubePrimitive(glm::vec3 scale) {
  return ZModel(ZPrimitiveType::Cube);
}

void ZModel::CreateCube(glm::vec3 scale) {

}

/**
 *  Sphere Creation
 */
ZModel ZModel::NewSpherePrimitive(glm::vec3 scale) {
  return ZModel(ZPrimitiveType::Sphere);
}

void ZModel::CreateSphere(glm::vec3 scale) {

}

/**
 *  Cylinder Creation
 */
ZModel ZModel::NewCylinderPrimitive(glm::vec3 scale) {
  return ZModel(ZPrimitiveType::Cylinder);
}

void ZModel::CreateCylinder(glm::vec3 scale) {

}

/**
 *  Cone Creation
 */
ZModel ZModel::NewConePrimitive(glm::vec3 scale) {
  return ZModel(ZPrimitiveType::Cone);
}

void ZModel::CreateCone(glm::vec3 scale) {

}
