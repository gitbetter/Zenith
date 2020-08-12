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
#include "ZResourceCache.hpp"

void ZLuaScriptManager::Initialize()
{
    lua_.open_libraries(sol::lib::base, sol::lib::package);
    lua_.set_function("executeFile", &ZLuaScriptManager::ExecuteFile, (*this));
    lua_.set_function("executeString", &ZLuaScriptManager::ExecuteString, (*this));
}

void ZLuaScriptManager::Load(std::shared_ptr<ZOFTree> zof)
{
    for (ZOFChildMap::iterator it = zof->children.begin(); it != zof->children.end(); it++)
    {
        if (it->first.find("ZSCR") == 0)
        {

            std::shared_ptr<ZOFObjectNode> scriptNode = std::static_pointer_cast<ZOFObjectNode>(it->second);

            ZOFPropertyMap props = scriptNode->properties;

            if (props.find("path") != props.end() && props["path"]->HasValues())
            {
                std::shared_ptr<ZOFString> prop = props["path"]->Value<ZOFString>(0);
                ZResource scriptResource(prop->value, ZResourceType::Script);
                zenith::ResourceCache()->GetHandle(&scriptResource);
                scripts_[it->first] = true;
            }
        }
    }
}

void ZLuaScriptManager::LoadAsync(std::shared_ptr<ZOFTree> zof)
{
    for (ZOFChildMap::iterator it = zof->children.begin(); it != zof->children.end(); it++)
    {
        if (it->first.find("ZSCR") == 0)
        {

            std::shared_ptr<ZOFObjectNode> scriptNode = std::static_pointer_cast<ZOFObjectNode>(it->second);

            ZOFPropertyMap props = scriptNode->properties;

            if (props.find("path") != props.end() && props["path"]->HasValues())
            {
                std::shared_ptr<ZOFString> prop = props["path"]->Value<ZOFString>(0);
                ZResource scriptResource(prop->value, ZResourceType::Script);
                zenith::ResourceCache()->RequestHandle(scriptResource);
                // TODO: Create a ZScriptReadyEvent that we can catch in order to cache the script properly.
                // Might also want to refactor the scripts into separate ZScript objects where we can store extra
                // data.
                scripts_[it->first] = true;
            }
        }
    }
}

void ZLuaScriptManager::ExecuteFile(const std::string& resource)
{
    lua_.script_file(resource);
}

void ZLuaScriptManager::ExecuteString(const std::string& script)
{
    lua_.script(script);
}
