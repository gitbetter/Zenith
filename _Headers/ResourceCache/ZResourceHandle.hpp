/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZResourceHandle.hpp

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
class ZResourceCache;
class ZResourceExtraData;

// Class and Data Structure Definitions
class ZResourceHandle {

  friend class ZResourceCache;

private:

public:

    ZResourceHandle(ZResource& resource, void* buffer, unsigned int size, ZResourceCache* resourceCache);
    virtual ~ZResourceHandle();

    unsigned int Size() const { return size_; }
    const void* Buffer() const { return buffer_; }
    void* FluidBuffer() { return buffer_; }
    std::shared_ptr<ZResourceExtraData> ExtraData() { return extraData_; }

		void SetExtra(std::shared_ptr<ZResourceExtraData> extra) { extraData_ = extra; }

protected:

  ZResource resource_;
  void* buffer_ = nullptr;
  unsigned int size_;
  ZResourceCache* resourceCache_ = nullptr;
  std::shared_ptr<ZResourceExtraData> extraData_;

};
