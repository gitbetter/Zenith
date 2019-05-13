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
#include "ZEngine.hpp"
#include "ZEvent.hpp"
#include "ZProcess.hpp"
#include "ZScriptableEvent.hpp"

// Forward Declarations
//class SomeClass;

// Class and Data Structure Definitions
const unsigned int NUM_EVENT_QUEUES = 2;
const unsigned int NUM_LISTENER_QUEUES = 2;


class ZScriptableEventAgent {

  typedef std::set<ZScriptableEventDelegate*> ScriptEventListeners;

  private:

    ScriptEventListeners listeners_;

  public:

    ~ZScriptableEventAgent();
    void AddListener(ZScriptableEventDelegate* listener);
    void DestroyListener(ZScriptableEventDelegate* listener);

};

class ZEventAgent : public ZProcess {

  typedef std::list<ZEventDelegate> EventListenerList;
  typedef std::map<ZEventType, EventListenerList> EventListenerMap;
  typedef std::list<std::shared_ptr<ZEvent>> EventQueue;

private:

  std::unique_ptr<ZScriptableEventAgent> scriptableEventAgent_;

  EventListenerMap eventListeners_[NUM_LISTENER_QUEUES];
  EventQueue eventQueues_[NUM_EVENT_QUEUES];
  int activeQueue_, activeListeners_;
  float updateTimeoutMax_;

public:

    ZEventAgent() : activeQueue_(0), activeListeners_(0), updateTimeoutMax_(ZEngine::UPDATE_STEP_SIZE * 2.f) { }
    ~ZEventAgent() { }

    void Initialize() override;
    void Update() override;

    bool AddListener(const ZEventDelegate& eventDelegate, const ZEventType& type);
    bool RemoveListener(const ZEventDelegate& eventDelegate, const ZEventType& type);
    bool TriggerEvent(const std::shared_ptr<ZEvent>& event);
    bool QueueEvent(const std::shared_ptr<ZEvent>& event);
    bool AbortEvent(const ZEventType& eventType, bool allOfType = false);

    ZScriptableEventAgent* Scriptable() const { return scriptableEventAgent_.get(); }

    void CleanUp() override;

protected:

};
