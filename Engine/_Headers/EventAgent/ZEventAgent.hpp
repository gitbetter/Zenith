/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZEventAgent.hpp

    Created by Adrian Sanchez on 12/03/2019.
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
#include "ZEvent.hpp"
#include "ZProcess.hpp"
#include "ZScriptableEvent.hpp"

// Forward Declarations
//class SomeClass;

// Class and Data Structure Definitions
const unsigned int NUM_EVENT_QUEUES = 2;
const unsigned int NUM_LISTENER_QUEUES = 2;

class ZScriptableEventAgent
{

    typedef std::set<ZScriptableEventDelegate*> ScriptEventListeners;

private:

    ScriptEventListeners listeners_;
    std::mutex agentMutex_;

public:

    ~ZScriptableEventAgent();
    void Subscribe(ZScriptableEventDelegate* listener);
    void DestroyListener(ZScriptableEventDelegate* listener);

};

class ZEventAgent : public ZProcess
{

    typedef std::list<std::shared_ptr<ZEventDelegate>> EventListenerList;
    typedef std::map<ZTypeIdentifier, EventListenerList> EventListenerMap;
    typedef std::list<std::shared_ptr<ZEvent>> EventQueue;

public:

    ZEventAgent() : activeQueue_(0), activeListeners_(0), updateTimeoutMax_(UPDATE_STEP_SIZE * 2.f) {}
    ~ZEventAgent() {}

    void Initialize() override;
    void Update(double deltaTime) override;

    bool Subscribe(const std::shared_ptr<ZEventDelegate>& eventDelegate, const ZTypeIdentifier& type);
    bool Unsubscribe(const std::shared_ptr<ZEventDelegate>& eventDelegate, const ZTypeIdentifier& type);
    bool Trigger(const std::shared_ptr<ZEvent>& event);
    bool Queue(const std::shared_ptr<ZEvent>& event);
    bool Cancel(const ZTypeIdentifier& eventType, bool allOfType = false);

    template<typename T, typename EventType>
    bool Subscribe(T* instance, void (T::*func)(const std::shared_ptr<EventType>&)) {
        if (!std::is_base_of<ZEvent, EventType>::value)
            return false;
        return Subscribe(
            std::make_shared<ZMemberEventDelegate<T, EventType>>(instance, func),
            EventType::Type
        );
    }

    template<typename EventType>
    bool Subscribe(void (*func)(const std::shared_ptr<EventType>&)) {
        if (!std::is_base_of<ZEvent, EventType>::value)
            return false;
        return Subscribe(
            std::make_shared<ZGlobalEventDelegate<EventType>>(func),
            EventType::Type
        );
    }

    template<typename T, typename EventType>
    bool Unsubscribe(T* instance, void (T::*func)(const std::shared_ptr<EventType>&)) {
        if (!std::is_base_of<ZEvent, EventType>::value)
            return false;
        return Unsubscribe(
            std::make_shared<ZMemberEventDelegate<T, EventType>>(instance, func),
            EventType::Type
        );
    }

    template<typename EventType>
    bool Unsubscribe(void (*func)(const std::shared_ptr<EventType>&)) {
        if (!std::is_base_of<ZEvent, EventType>::value)
            return false;
        return Unsubscribe(
            std::make_shared<ZGlobalEventDelegate<EventType>>(instance, func),
            EventType::Type
        );
    }

    ZScriptableEventAgent* Scriptable() const { return scriptableEventAgent_.get(); }

    void CleanUp() override;

private:

    std::unique_ptr<ZScriptableEventAgent> scriptableEventAgent_;

    EventListenerMap eventListeners_[NUM_LISTENER_QUEUES];
    EventQueue eventQueues_[NUM_EVENT_QUEUES];
    int activeQueue_, activeListeners_;
    float updateTimeoutMax_;

    struct {
        std::mutex listeners;
    } mutexes_;

};
