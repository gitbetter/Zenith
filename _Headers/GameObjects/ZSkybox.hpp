/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
 ZSkybox.hpp
 
 Created by Adrian Sanchez on 02/03/2019.
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
#include "ZGameObject.hpp"

// Forward Declarations
// class SomeClass;

// Class and Data Structure Definitions
class ZSkybox : public ZGameObject, public std::enable_shared_from_this<ZSkybox> {

private:

	std::string hdrPath_;
    
    void Initialize(ZTexture& cubeMap, ZBufferData& bufferData);
    
public:
    
    ZSkybox(std::string hdr = "");
    ~ZSkybox() { }
    
    void Initialize() override;
    void Initialize(std::shared_ptr<ZOFNode> root) override;
	void InitializeAsync();

	void Render(float frameMix, ZRenderOp renderOp = ZRenderOp::Color) override;
    
	bool IsVisible() override { return true; }
    
    ZIBLTexture IBLTexture() const { return iblTexture_; }
    
protected:
    
    ZIBLTexture iblTexture_;

	void HandleCubemapReady(std::shared_ptr<ZEvent> event);
    
};
