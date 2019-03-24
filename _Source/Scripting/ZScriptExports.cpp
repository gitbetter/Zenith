/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZScriptExports.cpp

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

#include "ZScriptExports.hpp"
#include "ZResource.hpp"
#include "ZEngine.hpp"
#include "ZLuaScriptManager.hpp"
#include "ZProcessRunner.hpp"
#include "ZResourceCache.hpp"

bool ZInternalScriptExports::Initialize() {

}

bool ZInternalScriptExports::Destroy() {

}

bool ZInternalScriptExports::LoadAndExecuteScriptResource(const std::string& scriptResource) {
  ZResource resource(scriptResource);
  std::shared_ptr<ZResourceHandle> handle = ZEngine::ResourceCache()->GetHandle(&resource);
  if (handle) {
    ZEngine::ScriptManager()->ExecuteString(std::string(handle->Buffer()));
    return true;
  }
  return false;
}

void ZInternalScriptExports::AttachScriptProcess(sol::table scriptProcess) {
  sol::object obj = scriptProcess["process"];
  if (obj.valid()) {
    std::shared_ptr<ZProcess> process = obj.as<std::shared_ptr<ZProcess>>();
    ZEngine::ProcessRunner()->AttachProcess(process);
  } else {
    _Z("Could not find 'process' object in script to attach", ZERROR);
  }
}

void ZScriptExports::Register() {
  sol::state& lua = ZEngine::ScriptManager()->LuaState();
  lua["loadAndExecuteScriptResource"] = ZInternalScriptExports::LoadAndExecuteScriptResource;
  lua["attachProcess"] = ZInternalScriptExports::AttachScriptProcess;
}

void ZScriptExports::UnRegister() {
  ZInternalScriptExports::Destroy();
}