/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZScriptableEvent.hpp

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

#pragma once

/*
  Note that with the scriptable event, we must call REGISTER_SCRIPT_EVENT somewhere in the game initialization
  code in order to register the event with Lua. We must also make sure to use the EXPORT_FOR_SCRIPT_EVENT
  macro somewhere near the end of any ZScriptableEvent subclass declarations so that the appropriate
  event creation function is included.
*/

#define REGISTER_SCRIPT_EVENT(eventClass, eventType)\
  ZScriptableEvent::RegisterEventTypeWithScript(#eventClass, eventType);\
  ZScriptableEvent::AddCreationFunction(eventType, &eventClass::CreateEventForScript);

#define EXPORT_FOR_SCRIPT_EVENT(eventClass, eventType)\
  public: \
  static ZScriptableEvent* CreateEventForScript() { \
    return new eventClass; \
  }

// Includes
#include "ZEvent.hpp"
#include "sol.hpp"

// Forward Declarations
class ZScriptableEvent;

// Class and Data Structure Definitions
using CreateEventForScriptFunctionType = ZScriptableEvent* (*)();

class ZScriptableEvent : public ZEvent
{

    using CreationFunctions = std::map<ZTypeIdentifier, CreateEventForScriptFunctionType>;

private:

    static CreationFunctions creationFunctions_;

public:

    static const ZTypeIdentifier Type;

    ZScriptableEvent() {}

    sol::table EventData();
    bool SetEventData(sol::table data);

    static void RegisterEventTypeWithScript(const std::string& key, ZTypeIdentifier type);
    static void AddCreationFunction(ZTypeIdentifier type, CreateEventForScriptFunctionType creationFunction);
    static ZScriptableEvent* CreateEventFromScript(ZTypeIdentifier type);

protected:

    sol::table eventData_;

    virtual bool BuildEventData() { return true; }
    virtual bool BuildEventFromScript() { return true; }

};

class ZScriptableEventDelegate
{

private:

    ZTypeIdentifier eventType_;
    sol::function scriptCallback_;
    std::shared_ptr<ZEventDelegate> delegate_;

public:

    explicit ZScriptableEventDelegate(ZTypeIdentifier eventType, const sol::function& scriptCallback)
        : eventType_(eventType), scriptCallback_(scriptCallback)
    {}
    ~ZScriptableEventDelegate() {}

    std::shared_ptr<ZEventDelegate> Delegate()
    {
        if (!delegate_) {
            delegate_ = std::make_shared<ZMemberEventDelegate<ZScriptableEventDelegate, ZScriptableEvent>>(this, &ZScriptableEventDelegate::EventDelegate);
        }
        return delegate_;
    }

    void EventDelegate(const std::shared_ptr<ZScriptableEvent>& event);

};