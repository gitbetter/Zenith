/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZDomainStrategy.hpp

    Created by Adrian Sanchez on 07/02/2019.
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
#include <glm/glm.hpp>

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
class ZDomainStrategy {
private:

public:

  ZDomainStrategy() { }
  virtual ~ZDomainStrategy() { }

  virtual void Initialize() = 0;
  virtual void CreateWindow(int width, int heights) = 0;
  virtual void PollEvents() = 0;
  virtual void CaptureCursor() = 0;
  virtual void ReleaseCursor() = 0;
  virtual void Resize(int width, int height) = 0;
  virtual glm::vec2 FramebufferSize() = 0;
  virtual bool IsWindowClosing() = 0;
  virtual void CloseWindow() = 0;
  virtual void CleanUp() = 0;

protected:

};
