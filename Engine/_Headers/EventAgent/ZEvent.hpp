/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZEvent.hpp

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
#include "ZCommon.hpp"

// Forward Declarations
//class SomeClass;

// Class and Data Structure Definitions
class ZEventDelegate {

public:

    void operator()(const std::shared_ptr<ZEvent>& event) const {
        Call(event);
    }
    bool operator==(const ZEventDelegate& other) const {
        return IsEqualTo(other);
    }
    bool operator!=(const ZEventDelegate& other) const {
        return !IsEqualTo(other);
    }

protected:

    virtual void Call(const std::shared_ptr<ZEvent>& event) const = 0;
    virtual bool IsEqualTo(const ZEventDelegate& other) const = 0;

};

template<class T, class EventType>
class ZMemberEventDelegate : public ZEventDelegate {

public:

    using MemberFunction = void (T::*)(const std::shared_ptr<EventType>&);

    ZMemberEventDelegate(T* instance, MemberFunction func)
        : instance_(instance), func_(func)
    { }

    void Call(const std::shared_ptr<ZEvent>& event) const override {
        (instance_->*func_)(std::dynamic_pointer_cast<EventType>(event));
    }

    bool IsEqualTo(const ZEventDelegate& other) const  override {
        if (ZMemberEventDelegate const* otherMemberDelegate = dynamic_cast<ZMemberEventDelegate const*>(&other)) {
            return otherMemberDelegate->instance_ == instance_ && otherMemberDelegate->func_ == func_;
        }
        return false;
    }

protected:

    T* instance_;
    MemberFunction func_;

};

template<class EventType>
class ZGlobalEventDelegate : public ZEventDelegate {

public:

    using GlobalFunction = void (*)(const std::shared_ptr<EventType>&);

    ZGlobalEventDelegate(GlobalFunction func) : func_(func)
    { }

    void Call(const std::shared_ptr<ZEvent>& event) const override {
        (*func_)(std::dynamic_pointer_cast<EventType>(event));
    }

    bool IsEqualTo(const ZEventDelegate& other) const  override {
        if (ZGlobalEventDelegate const* otherMemberDelegate = dynamic_cast<ZGlobalEventDelegate const*>(&other)) {
            return otherMemberDelegate->func_ == func_;
        }
        return false;
    }

protected:

    GlobalFunction func_;

};

class ZEvent
{

private:

    float timeStamp_;

public:

    static const ZTypeIdentifier Type;

    explicit ZEvent(const float timeStamp = 0.f) : timeStamp_(timeStamp) {}
    virtual ~ZEvent() {}

    virtual const ZTypeIdentifier& EventType() const { return Type; }
    virtual float TimeStamp() const { return timeStamp_; }
    virtual void SetTimeStamp(float timestamp) { timeStamp_ = timestamp; }
    virtual void Serialize(std::ostringstream& out) const {}
    virtual std::shared_ptr<ZEvent> Copy() const = 0;
    virtual std::string Name() const = 0;

protected:

};
