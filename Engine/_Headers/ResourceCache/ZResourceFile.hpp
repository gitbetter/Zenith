/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZResourceFile.hpp

    Created by Adrian Sanchez on 09/03/2019.
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
#include "ZResource.hpp"

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
class ZResourceFile {

public:

  virtual ~ZResourceFile() { }
  virtual bool Open() = 0;
  virtual const std::string& Name() const = 0;
  virtual unsigned int RawResourceSize(ZResource& resource) = 0;
  virtual unsigned int RawResource(ZResource& resource, char* buffer) = 0;
  virtual unsigned int ResourceCount() const = 0;
  virtual std::string ResourceName(unsigned int num) const = 0;
  virtual void PrintResources() const = 0;
  virtual void Close() = 0;

protected:

};
