/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZDomain.hpp
 
    Created by Adrian Sanchez on 27/01/2019.
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
#include "ZDomainStrategy.hpp"
#include "ZEngine.hpp"

// Forward Declarations

// Class and Data Structure Definitions
class ZDomain {
    
public:
    
    ZDomain(unsigned int windowWidth, unsigned int windowHeight) : windowWidth_(windowWidth), windowHeight_(windowHeight) { }
    virtual ~ZDomain() { }
    
    void Initialize();
    
    unsigned int WindowWidth() const { return windowWidth_; }
    unsigned int WindowHeight() const { return windowHeight_; }
    unsigned int ResolutionX() const { return resolutionX_; }
    unsigned int ResolutionY() const { return resolutionY_; }
    float ResolutionXRatio() const { return (float)ResolutionX() / zenith::DEFAULT_X_RESOLUTION; }
    float ResolutionYRatio() const { return (float)ResolutionY() / zenith::DEFAULT_Y_RESOLUTION; }
    float Aspect() const { return (float)ResolutionX() / (float)ResolutionY(); }
    
    ZDomainStrategy* Strategy() { return domainStrategy_.get(); }
	void* MainContext() { return mainContext_; }
    
    void ResizeWindow(int width, int height);
    
    void CleanUp();
    
protected:
    
    std::unique_ptr<ZDomainStrategy> domainStrategy_;
    unsigned int windowWidth_, windowHeight_;
	unsigned int resolutionX_, resolutionY_;
	void* mainContext_;

};
