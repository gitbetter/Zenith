/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZLuaScriptManager.cpp

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

#include "ZLuaScriptManager.hpp"
#include "ZResource.hpp"
#include "ZResourceImporter.hpp"
#include "ZServices.hpp"

void ZLuaScriptManager::Initialize()
{
    lua_.open_libraries(sol::lib::base, sol::lib::package);
    lua_.set_function("executeFile", &ZLuaScriptManager::ExecuteFile, (*this));
    lua_.set_function("executeString", &ZLuaScriptManager::ExecuteString, (*this));
    lua_["log"] = [](const std::string& message) { ILOG(message); };
}

void ZLuaScriptManager::ExecuteFile(const std::string& resource)
{
    lua_.script_file(resource);
}

void ZLuaScriptManager::ExecuteString(const std::string& script)
{
    lua_.script(script);
}
