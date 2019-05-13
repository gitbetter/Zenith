/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZResourceExtraData.hpp

    Created by Adrian Sanchez on 11/04/2019.
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

// Forward Declarations
class ZWavResourceLoader;
class ZOggResourceLoader;

// Class and Data Structure Definitions
class ZResourceExtraData {

public:

	virtual ~ZResourceExtraData() { }

};

class ZSoundResourceExtraData : public ZResourceExtraData {

	friend class ZWavResourceLoader;
	friend class ZOggResourceLoader;

public:

    ZSoundResourceExtraData();
    ~ZSoundResourceExtraData() { }

    std::string ToString() { return "ZSoundResourceExtraData"; }
    ZSoundType SoundType() { return soundType_; }
    unsigned int LengthMilli() const { return lengthMilli_; }
		ZWavFormatDesc* WavFormatDesc() { return &wavFormatDesc_; }
    
protected:

    ZSoundType soundType_;
    unsigned int lengthMilli_;
		ZWavFormatDesc wavFormatDesc_;

};