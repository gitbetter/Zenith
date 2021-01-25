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
#include "ZLuaScriptManager.hpp"
#include "ZProcessRunner.hpp"
#include "ZScriptableProcess.hpp"
#include "ZResourceCache.hpp"
#include "ZScriptableEvent.hpp"
#include "ZEventAgent.hpp"

bool ZInternalScriptExports::LoadAndExecuteScriptResource(const std::string& scriptResource)
{
    ZResource resource(scriptResource, ZResourceType::Script);
    std::shared_ptr<ZResourceHandle> handle = zenith::ResourceCache()->GetHandle(&resource);
    return handle != nullptr;
}

void ZInternalScriptExports::AttachScriptProcess(std::shared_ptr<ZScriptableProcess> scriptProcess)
{
    if (scriptProcess)
    {
        zenith::ProcessRunner()->AttachProcess(scriptProcess, ZPriority::Low);
    }
    else
    {
        zenith::Log("Could not find 'process' object in script to attach", ZSeverity::Error);
    }
}

bool ZInternalScriptExports::QueueEvent(ZEventType eventType, const sol::table& eventData)
{
    std::shared_ptr<ZScriptableEvent> event(BuildEvent(eventType, eventData));
    if (event)
    {
        return zenith::EventAgent()->QueueEvent(event);
    }
    return false;
}

bool ZInternalScriptExports::TriggerEvent(ZEventType eventType, const sol::table& eventData)
{
    std::shared_ptr<ZScriptableEvent> event(BuildEvent(eventType, eventData));
    if (event)
    {
        return zenith::EventAgent()->TriggerEvent(event);
    }
    return false;
}

std::shared_ptr<ZScriptableEvent> ZInternalScriptExports::BuildEvent(ZEventType eventType, const sol::table& eventData)
{
    std::shared_ptr<ZScriptableEvent> event(ZScriptableEvent::CreateEventFromScript(eventType));
    if (!event) return nullptr;
    if (!event->SetEventData(eventData)) return nullptr;
    return event;
}

unsigned long ZInternalScriptExports::RegisterEventListener(ZEventType eventType, const sol::function& callback)
{
    if (callback.valid())
    {
        ZScriptableEventDelegate* listener = new ZScriptableEventDelegate(eventType, callback);
        zenith::EventAgent()->Scriptable()->AddListener(listener);
        zenith::EventAgent()->AddListener(listener->Delegate(), eventType);
        unsigned long handle = reinterpret_cast<unsigned long>(listener);
        return handle;
    }

    zenith::Log("Could not register the script event delegate using an invalid callback", ZSeverity::Error);
    return 0;
}

void ZInternalScriptExports::Log(const sol::object& obj)
{
    if (obj.is<std::string>())
    {
        zenith::Log(obj.as<std::string>(), ZSeverity::Error);
    }
    else
    {
        zenith::Log("<Sol Type " + std::to_string((int) obj.get_type()) + ">", ZSeverity::Error);
    }
}

void ZScriptExports::Register()
{
    ZLuaScriptManager* scriptManager = static_cast<ZLuaScriptManager*>(zenith::ScriptManager());
    sol::state& lua = scriptManager->LuaState();
    lua["LoadAndExecuteScriptResource"] = ZInternalScriptExports::LoadAndExecuteScriptResource;
    lua["AttachProcess"] = ZInternalScriptExports::AttachScriptProcess;
    lua["QueueEvent"] = ZInternalScriptExports::QueueEvent;
    lua["TriggerEvent"] = ZInternalScriptExports::TriggerEvent;
    lua["RegisterEventListener"] = ZInternalScriptExports::RegisterEventListener;
    lua["Log"] = ZInternalScriptExports::Log;
}

void ZScriptExports::UnRegister()
{
    ZInternalScriptExports::Destroy();
}
