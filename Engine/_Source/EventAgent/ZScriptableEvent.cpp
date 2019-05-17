/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZScriptableEvent.cpp

    Created by Adrian Sanchez on 24/03/2019.
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

#include "ZScriptableEvent.hpp"
#include "ZLuaScriptManager.hpp"
#include "ZEngine.hpp"

ZScriptableEvent::CreationFunctions ZScriptableEvent::creationFunctions_;

sol::table ZScriptableEvent::EventData() {
  if (!eventData_.valid()) {
    BuildEventData();
  }
  return eventData_;
}

bool ZScriptableEvent::SetEventData(sol::table data) { 
  if (!data.valid()) return false;
  eventData_ = data;
  return BuildEventFromScript();
}

void ZScriptableEvent::RegisterEventTypeWithScript(const std::string& key, ZEventType type) {
  sol::state& lua = zenith::ScriptManager()->LuaState();
  sol::table eventTypeTable = lua["ZEventType"];
  if (!eventTypeTable.valid())
    eventTypeTable = lua.create_named_table("ZEventType");
  
  eventTypeTable[key] = (double)type;
}

void ZScriptableEvent::AddCreationFunction(ZEventType type, CreateEventForScriptFunctionType creationFunction) {
  creationFunctions_[type] = creationFunction;
}

ZScriptableEvent* ZScriptableEvent::CreateEventFromScript(ZEventType type) {
  if (creationFunctions_.find(type) == creationFunctions_.end()) return nullptr;
  return creationFunctions_[type]();
}

void ZScriptableEventDelegate::EventDelegate(std::shared_ptr<ZEvent> event) {
  std::shared_ptr<ZScriptableEvent> scriptEvent = std::static_pointer_cast<ZScriptableEvent>(event);
  sol::function callback = scriptCallback_;
  callback(scriptEvent->EventData());
}
