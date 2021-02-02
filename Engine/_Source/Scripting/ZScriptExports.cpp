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

#include "ZServices.hpp"
#include "ZScriptExports.hpp"
#include "ZResource.hpp"
#include "ZLuaScriptManager.hpp"
#include "ZScriptableProcess.hpp"
#include "ZScriptableEvent.hpp"

bool ZInternalScriptExports::LoadAndExecuteScriptResource(const std::string& scriptResource)
{
    ZResource resource(scriptResource, ZResourceType::Script);
    std::shared_ptr<ZResourceHandle> handle = ZServices::ResourceCache()->GetHandle(&resource);
    return handle != nullptr;
}

void ZInternalScriptExports::AttachScriptProcess(const std::shared_ptr<ZScriptableProcess>& scriptProcess, const std::shared_ptr<ZProcessRunner>& processRunner)
{
    if (scriptProcess && processRunner)
    {
        processRunner->AttachProcess(scriptProcess, ZPriority::Low);
    }
    else
    {
        LOG("Could not find 'scriptProcess' or 'processRunner' object in script to attach", ZSeverity::Error);
    }
}

bool ZInternalScriptExports::Queue(ZTypeIdentifier eventType, const sol::table& eventData)
{
    std::shared_ptr<ZScriptableEvent> event(BuildEvent(eventType, eventData));
    if (event)
    {
        return ZServices::EventAgent()->Queue(event);
    }
    return false;
}

bool ZInternalScriptExports::Trigger(ZTypeIdentifier eventType, const sol::table& eventData)
{
    std::shared_ptr<ZScriptableEvent> event(BuildEvent(eventType, eventData));
    if (event)
    {
        return ZServices::EventAgent()->Trigger(event);
    }
    return false;
}

std::shared_ptr<ZScriptableEvent> ZInternalScriptExports::BuildEvent(ZTypeIdentifier eventType, const sol::table& eventData)
{
    std::shared_ptr<ZScriptableEvent> event(ZScriptableEvent::CreateEventFromScript(eventType));
    if (!event) return nullptr;
    if (!event->SetEventData(eventData)) return nullptr;
    return event;
}

unsigned long ZInternalScriptExports::RegisterEventListener(ZTypeIdentifier eventType, const sol::function& callback)
{
    if (callback.valid())
    {
        ZScriptableEventDelegate* listener = new ZScriptableEventDelegate(eventType, callback);
        ZServices::EventAgent()->Scriptable()->Subscribe(listener);
        //ZServices::EventAgent()->Subscribe(listener->Delegate(), eventType);
        unsigned long handle = reinterpret_cast<unsigned long>(listener);
        return handle;
    }

    LOG("Could not register the script event delegate using an invalid callback", ZSeverity::Error);
    return 0;
}

void ZInternalScriptExports::Log(const sol::object& obj)
{
    if (obj.is<std::string>())
    {
        LOG(obj.as<std::string>(), ZSeverity::Error);
    }
    else
    {
        LOG("<Sol Type " + std::to_string((int) obj.get_type()) + ">", ZSeverity::Error);
    }
}

void ZScriptExports::Register()
{
    std::shared_ptr<ZLuaScriptManager> scriptManager = std::dynamic_pointer_cast<ZLuaScriptManager>(ZServices::ScriptManager());
    sol::state& lua = scriptManager->LuaState();
    lua["LoadAndExecuteScriptResource"] = ZInternalScriptExports::LoadAndExecuteScriptResource;
    lua["AttachProcess"] = ZInternalScriptExports::AttachScriptProcess;
    lua["Queue"] = ZInternalScriptExports::Queue;
    lua["Trigger"] = ZInternalScriptExports::Trigger;
    lua["RegisterEventListener"] = ZInternalScriptExports::RegisterEventListener;
    lua["Log"] = ZInternalScriptExports::Log;
}

void ZScriptExports::UnRegister()
{
    ZInternalScriptExports::Destroy();
}
