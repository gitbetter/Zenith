/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZLuaScriptManager.hpp

    Created by Adrian Sanchez on 23/03/2019.
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
#include "ZScriptManager.hpp"
#include "sol.hpp"

// Forward Declarations


// Class and Data Structure Definitions
class ZLuaScriptManager : public ZScriptManager {

private:

  sol::state lua_;
  std::string lastError_;

public:

  ZLuaScriptManager() { }
  // sol::state has deleted copy constructor, so 
  // we must define one in order to bypass deletion of ZLuaScriptManager copy constructor
  ZLuaScriptManager(ZLuaScriptManager& mgr) { }
  ~ZLuaScriptManager() { }

  sol::state& LuaState() { return lua_; }

  virtual bool Initialize() override;
  virtual bool Load(std::shared_ptr<ZOFTree> zof) override;
  virtual void ExecuteFile(const std::string& resource) override;
  virtual void ExecuteString(const std::string& script) override;

};