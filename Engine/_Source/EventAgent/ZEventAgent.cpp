/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZEventAgent.cpp

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

#include "ZEventAgent.hpp"
#include "ZServices.hpp"

void ZEventAgent::Initialize()
{
    ZProcess::Initialize();
}

bool ZEventAgent::Subscribe(const std::shared_ptr<ZEventDelegate>& eventDelegate, const ZTypeIdentifier& type)
{
    std::lock_guard<std::mutex> listenersLock(mutexes_.listeners);
    EventListenerList& listeners = eventListeners_[activeListeners_][type];
    for (auto it = listeners.begin(); it != listeners.end(); it++)
    {
        if (eventDelegate == (*it))
        {
            LOG("Attempted to register the same delegate twice for event " + std::to_string(type), ZSeverity::Warning);
            return false;
        }
    }
    listeners.emplace_back(eventDelegate);
    return true;
}

bool ZEventAgent::Unsubscribe(const std::shared_ptr<ZEventDelegate>& eventDelegate, const ZTypeIdentifier& type)
{
    bool success = false;

    {
        std::lock_guard<std::mutex> listenersLock(mutexes_.listeners);
        auto findIt = eventListeners_[activeListeners_].find(type);
        if (findIt != eventListeners_[activeListeners_].end())
        {
            EventListenerList& listeners = findIt->second;
            for (auto it = listeners.begin(); it != listeners.end(); it++)
            {
                if ((*it) == eventDelegate)
                {
                    listeners.erase(it);
                    success = true;
                    break;
                }
            }
        }
    }

    return success;
}

bool ZEventAgent::Trigger(const std::shared_ptr<ZEvent>& event)
{
    bool processed = false;

    {
        std::lock_guard<std::mutex> listenersLock(mutexes_.listeners);
        auto findIt = eventListeners_[activeListeners_].find(event->EventType());
        if (findIt != eventListeners_[activeListeners_].end())
        {
            const EventListenerList& listeners = findIt->second;
            for (auto it = listeners.begin(); it != listeners.end(); it++)
            {
                event->SetTimeStamp(SECONDS_TIME);
                (*it)->operator()(event);
                processed = true;
            }
        }
    }

    return processed;
}

bool ZEventAgent::Queue(const std::shared_ptr<ZEvent>& event)
{
    assert(activeQueue_ >= 0 && activeQueue_ < NUM_EVENT_QUEUES);
    bool success = false;

    {
        std::lock_guard<std::mutex> listenersLock(mutexes_.listeners);
        auto findIt = eventListeners_[activeListeners_].find(event->EventType());
        if (findIt != eventListeners_[activeListeners_].end())
        {
            eventQueues_[activeQueue_].emplace_back(event);
            success = true;
        }
    }

    return success;
}

bool ZEventAgent::Cancel(const ZTypeIdentifier& eventType, bool allOfType)
{
    assert(activeQueue_ >= 0 && activeQueue_ < NUM_EVENT_QUEUES);
    bool success = false;

    {
        std::lock_guard<std::mutex> listenersLock(mutexes_.listeners);
        auto findIt = eventListeners_[activeListeners_].find(eventType);
        if (findIt != eventListeners_[activeListeners_].end())
        {
            EventQueue eventQueue = eventQueues_[activeQueue_];
            auto it = eventQueue.begin();
            while (it != eventQueue.end())
            {
                auto thisIt = it;
                ++it;
                if ((*thisIt)->EventType() == eventType)
                {
                    eventQueue.erase(thisIt);
                    success = true;
                    if (!allOfType) break;
                }
            }
        }
    }

    return success;
}

void ZEventAgent::Update(double deltaTime)
{
    constexpr float floatMax = std::numeric_limits<float>::max();
    float currentTime = SECONDS_TIME;
    const float maxTime = ((updateTimeoutMax_ == floatMax) ? floatMax : currentTime + updateTimeoutMax_);

    int listenersToProcess = activeListeners_;
    int queueToProcess = activeQueue_;
    {
        std::lock_guard<std::mutex> listenersLock(mutexes_.listeners);
        activeListeners_ = (activeListeners_ + 1) % NUM_LISTENER_QUEUES;
        eventListeners_[activeListeners_] = eventListeners_[listenersToProcess];

        activeQueue_ = (activeQueue_ + 1) % NUM_EVENT_QUEUES;
        eventQueues_[activeQueue_].clear();
    }

    while (!eventQueues_[queueToProcess].empty())
    {
        std::shared_ptr<ZEvent> event = eventQueues_[queueToProcess].front();
        eventQueues_[queueToProcess].pop_front();
        const ZTypeIdentifier& eventType = event->EventType();

        auto findIt = eventListeners_[listenersToProcess].find(eventType);
        if (findIt != eventListeners_[listenersToProcess].end())
        {
            const EventListenerList& listeners = findIt->second;
            for (auto it = listeners.begin(); it != listeners.end(); it++)
            {
                event->SetTimeStamp(SECONDS_TIME);
                (*it)->operator()(event);
            }
        }

        currentTime = SECONDS_TIME;
        if (updateTimeoutMax_ != floatMax && currentTime >= maxTime)
        {
            break;
        }
    }

    {
        std::lock_guard<std::mutex> listenersLock(mutexes_.listeners);
        bool queueFlushed = eventQueues_[queueToProcess].empty();
        if (!queueFlushed)
        {
            while (!eventQueues_[queueToProcess].empty())
            {
                std::shared_ptr<ZEvent> event = eventQueues_[queueToProcess].back();
                eventQueues_[queueToProcess].pop_back();
                eventQueues_[activeQueue_].emplace_front(event);
            }
        }

        eventListeners_[listenersToProcess] = eventListeners_[activeListeners_];
    }

    ZProcess::Update(deltaTime);
}

void ZEventAgent::CleanUp()
{
    {
        std::lock_guard<std::mutex> listenersLock(mutexes_.listeners);
        for (int i = 0; i < NUM_LISTENER_QUEUES; i++)
        {
            eventListeners_[i].clear();
        }
    }
}
