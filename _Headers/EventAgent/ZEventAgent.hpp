//
//  ZEventAgent.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 12/03/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#pragma once

// Includes
#include "ZEvent.hpp"

// Forward Declarations
//class SomeClass;

// Class and Data Structure Definitions
const unsigned int NUM_EVENT_QUEUES = 2;

class ZEventAgent {

  typedef std::list<ZEventDelegate> EventListenerList;
  typedef std::map<ZEventType, EventListenerList> EventListenerMap;
  typedef std::list<std::shared_ptr<ZEvent>> EventQueue;

private:

  EventListenerMap eventListeners_;
  EventQueue eventQueues_[NUM_EVENT_QUEUES];
  int activeQueue_;

public:

    ZEventAgent() : activeQueue_(0) { }
    ~ZEventAgent() { }

    bool AddListener(const ZEventDelegate& eventDelegate, const ZEventType& type);
    bool RemoveListener(const ZEventDelegate& eventDelegate, const ZEventType& type);
    bool TriggerEvent(const std::shared_ptr<ZEvent>& event);
    bool QueueEvent(const std::shared_ptr<ZEvent>& event);
    bool AbortEvent(const ZEventType& eventType, bool allOfType = false);
    bool Process(float timeoutMax = std::numeric_limits<float>::max());

protected:

};
