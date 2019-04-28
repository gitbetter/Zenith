/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZMesh2D.cpp

    Created by Adrian Sanchez on 06/02/2019.
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

#include "ZUI.hpp"
#include "ZMesh2D.hpp"
#include "ZEngine.hpp"
#include "ZGraphicsStrategy.hpp"
#include "ZShader.hpp"

ZMesh2D::ZMesh2D(std::vector<ZVertex2D> vertices) : vertices_(vertices) {
  bufferData_ = ZEngine::Graphics()->Strategy()->LoadVertexData(vertices);
}

void ZMesh2D::Render(ZShader* shader, ZMaterial* material) {
  shader->Activate();
  ZEngine::Graphics()->Strategy()->Draw(bufferData_, vertices_);
}
