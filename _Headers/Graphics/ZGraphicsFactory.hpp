//
//  ZGraphicsFactory.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 19/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZCommon.hpp"
#include "ZGraphics.hpp"

// Forward Declarations
class ZModel;

// Class and Data Structure Definitions
class ZGraphicsFactory {
  typedef ZModel* (*ZModelCreator)(glm::vec3, std::vector<ZTexture>);
private:

public:

  ZGraphicsFactory();
  ~ZGraphicsFactory() { }

  ZShaderMap CreateShaders(ZOFTree* data);
  ZTextureMap CreateTextures(ZOFTree* data);

  ZModel* CreateModel(std::string type, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), std::vector<ZTexture> textures = {});

protected:

  std::map<std::string, ZModelCreator> modelCreators_;
};
