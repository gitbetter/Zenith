/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZGraphicsFactory.hpp

    Created by Adrian Sanchez on 19/02/2019.
    Copyright © 2019 Pervasive Sense. All rights reserved.

  This file is part of Zenith.

  Zenith is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Zenith is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Zenith.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

// Includes
#include "ZCommon.hpp"
#include "ZGraphics.hpp"

// Forward Declarations
class ZModel;

// Class and Data Structure Definitions
class ZGraphicsFactory {
  typedef std::unique_ptr<ZModel> (*ZModelCreator)(glm::vec3);
private:

public:

  ZGraphicsFactory();
  ~ZGraphicsFactory() { }

  ZShaderMap CreateShaders(ZOFTree* data);
  ZTextureMap CreateTextures(ZOFTree* data);

  std::unique_ptr<ZModel> CreateModel(std::string type, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f));

protected:

  std::map<std::string, ZModelCreator> modelCreators_;
};
