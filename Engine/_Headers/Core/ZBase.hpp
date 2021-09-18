/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZBase.hpp

    Created by Adrian Sanchez on 1/25/19.
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

#include "ZCommon.hpp"

class ZDomain;
class ZPhysicsUniverse;
class ZAudio;

class ZBase
{

public:

    ZBase(const std::string& name = "Untitled")
        : name_(name)
    { }
    virtual ~ZBase() = default;

    void Initialize(int windowWidth, int windowHeight);
    void Initialize(const ZGameOptions& options);

    std::shared_ptr<ZDomain> Domain() const { return gameSystems_.domain; }
    std::shared_ptr<ZPhysicsUniverse> PhysicsUniverse() const { return gameSystems_.physics; }
    std::shared_ptr<ZAudio> Audio() const { return gameSystems_.audio; }
    const ZGameOptions& Options() const { return gameOptions_; }
    const std::string& Name() const { return name_; }

    void SetName(const std::string& name) { name_ = name; }

    virtual void Provide(const std::shared_ptr<ZDomain>& domain);
    virtual void Provide(const std::shared_ptr<ZPhysicsUniverse>& physics);
    virtual void Provide(const std::shared_ptr<ZAudio>& audio);
    virtual void Configure(const ZGameOptions& options);

    virtual void CleanUp();

protected:

    ZGameSystems gameSystems_;
    ZGameOptions gameOptions_;
    std::string name_;

    virtual void Setup() = 0;

};
