//
//  ZShader.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 26/01/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include <string>

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
enum ZShaderTypes {
  Vertex, Pixel, Tesselation, Geometry, Other
};

class ZShader {
private:
  unsigned int id_;

  std::string GetShaderCode(const std::string& shaderPath);
  int CompileShader(const std::string& shaderCode, ZShaderTypes shaderType);
  unsigned int CreateProgram(int vShader, int pShader, int gShader);
  void CheckCompileErrors(unsigned int compilationUnit, ZShaderTypes shaderType);

public:
  ZShader(const std::string& vertexShaderPath, const std::string& pixelShaderPath, const std::string& geomShaderPath);
  ~ZShader() { }

  unsigned int getID() { return id_; }
protected:

};
