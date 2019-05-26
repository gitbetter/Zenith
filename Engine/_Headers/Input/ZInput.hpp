/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZInput.hpp
 
    Created by Adrian Sanchez on 28/01/2019.
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
#include "ZProcess.hpp"

// Forward Declarations
class ZObject;

// Class and Data Structure Definitions
class ZInput : public ZProcess {
    
public:
    
    virtual ~ZInput() { }
    
    virtual void Initialize() { ZProcess::Initialize(); }
    virtual void GetCursorPosition(double& x, double& y) = 0;
    virtual void SetCursorPosition(double& x, double& y) = 0;
    
    virtual bool Key(unsigned int key) { return key < ZKEY_LAST+1 && key >= 0 && keyPress_[key]; }
    virtual bool Mouse(unsigned int mouse) { return mouse < ZMOUSE_LAST+1 && mouse >= 0 && mousePress_[mouse]; }
    
protected:
    
    bool keyPress_[ZKEY_LAST+1] = { false };
    bool mousePress_[ZMOUSE_LAST+1] = { false };
    
};
