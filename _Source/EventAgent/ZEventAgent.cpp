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

void ZEventAgent::Initialize() {
	ZProcess::Initialize();
	scriptableEventAgent_.reset(new ZScriptableEventAgent);
}

bool ZEventAgent::AddListener(const ZEventDelegate& eventDelegate, const ZEventType& type) {
	EventListenerList& listeners = eventListeners_[activeListeners_][type];
	for (auto it = listeners.begin(); it != listeners.end(); it++) {
		if (eventDelegate == (*it)) {
			_Z("Attempted to register the same delegate twice", ZERROR);
			return false;
		}
	}

	listeners.push_back(eventDelegate);
	return true;
}

bool ZEventAgent::RemoveListener(const ZEventDelegate& eventDelegate, const ZEventType& type) {
	bool success = false;

	auto findIt = eventListeners_[activeListeners_].find(type);
	if (findIt != eventListeners_[activeListeners_].end()) {
		EventListenerList& listeners = findIt->second;
		for (auto it = listeners.begin(); it != listeners.end(); it++) {
			if ((*it) == eventDelegate) {
				listeners.erase(it);
				success = true;
				break;
			}
		}
	}

	return success;
}

bool ZEventAgent::TriggerEvent(const std::shared_ptr<ZEvent>& event) {
	bool processed = false;

	auto findIt = eventListeners_[activeListeners_].find(event->EventType());
	if (findIt != eventListeners_[activeListeners_].end()) {
		const EventListenerList& listeners = findIt->second;
		for (auto it = listeners.begin(); it != listeners.end(); it++) {
			ZEventDelegate delegate = (*it);
			event->SetTimeStamp(ZEngine::SecondsTime());
			delegate(event);
			processed = true;
		}
	}

	return processed;
}

bool ZEventAgent::QueueEvent(const std::shared_ptr<ZEvent>& event) {
	assert(activeQueue_ >= 0 && activeQueue_ < NUM_EVENT_QUEUES);
	auto findIt = eventListeners_[activeListeners_].find(event->EventType());
	if (findIt != eventListeners_[activeListeners_].end()) {
		eventQueues_[activeQueue_].push_back(event);
		return true;
	}
	return false;
}

bool ZEventAgent::AbortEvent(const ZEventType& eventType, bool allOfType) {
	assert(activeQueue_ >= 0 && activeQueue_ < NUM_EVENT_QUEUES);

	bool success = false;
	auto findIt = eventListeners_[activeListeners_].find(eventType);
	if (findIt != eventListeners_[activeListeners_].end()) {
		EventQueue eventQueue = eventQueues_[activeQueue_];
		auto it = eventQueue.begin();
		while (it != eventQueue.end()) {
			auto thisIt = it;
			++it;
			if ((*thisIt)->EventType() == eventType) {
				eventQueue.erase(thisIt);
				success = true;
				if (!allOfType) break;
			}
		}
	}

	return success;
}

void ZEventAgent::Update() {
	float floatMax = std::numeric_limits<float>::max();
	float currentTime = ZEngine::SecondsTime();
	float maxTime = ((updateTimeoutMax_ == floatMax) ? floatMax : currentTime + updateTimeoutMax_);

	int queueToProcess = activeQueue_;
	int listenersToProcess = activeListeners_;
	activeQueue_ = (activeQueue_ + 1) % NUM_EVENT_QUEUES;
	activeListeners_ = (activeListeners_ + 1) % NUM_LISTENER_QUEUES;

	eventQueues_[activeQueue_].clear();
	eventListeners_[activeListeners_] = eventListeners_[listenersToProcess];

	while (!eventQueues_[queueToProcess].empty()) {
		std::shared_ptr<ZEvent> event = eventQueues_[queueToProcess].front();
		eventQueues_[queueToProcess].pop_front();
		const ZEventType& eventType = event->EventType();

		auto findIt = eventListeners_[listenersToProcess].find(eventType);
		if (findIt != eventListeners_[listenersToProcess].end()) {
			const EventListenerList& listeners = findIt->second;
			for (auto it = listeners.begin(); it != listeners.end(); it++) {
				event->SetTimeStamp(ZEngine::SecondsTime());
				(*it)(event);
			}
		}

		currentTime = ZEngine::SecondsTime();
		if (updateTimeoutMax_ != floatMax && currentTime >= maxTime) {
			_Z("ZEventAgent processing timeout", ZWARNING);
			break;
		}
	}

	bool queueFlushed = eventQueues_[queueToProcess].empty();
	if (!queueFlushed) {
		while (!eventQueues_[queueToProcess].empty()) {
			std::shared_ptr<ZEvent> event = eventQueues_[queueToProcess].back();
			eventQueues_[queueToProcess].pop_back();
			eventQueues_[activeQueue_].push_front(event);
		}
	}

	eventListeners_[listenersToProcess] = eventListeners_[activeListeners_];

	ZProcess::Update();
}

void ZEventAgent::CleanUp() {
	for (int i = 0; i < NUM_LISTENER_QUEUES; i++) {
		eventListeners_[i].clear();
	}
	scriptableEventAgent_.reset();
}

ZScriptableEventAgent::~ZScriptableEventAgent() {
	for (auto it = listeners_.begin(); it != listeners_.end(); it++) {
		ZScriptableEventDelegate* listener = (*it);
		delete listener;
	}
	listeners_.clear();
}

void ZScriptableEventAgent::AddListener(ZScriptableEventDelegate* listener) {
	listeners_.insert(listener);
}

void ZScriptableEventAgent::DestroyListener(ZScriptableEventDelegate* listener) {
	ScriptEventListeners::iterator foundIt = listeners_.find(listener);
	if (foundIt != listeners_.end()) {
		listeners_.erase(foundIt);
		delete listener;
	}
}
