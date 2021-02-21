/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZCollider.hpp

    Created by Adrian Sanchez on 16/04/2019.
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

// Class and Data Structure Definitions
class ZCollider
{

public:

    ZCollider(ZColliderType type, const glm::vec3& extents, const glm::vec3& offset = glm::vec3(0.f))
        : type_(type), extents_(extents), offset_(offset)
    { }
    virtual ~ZCollider() {}

    virtual void Initialize() = 0;

    void* Get() const { return ptr_; }
    ZColliderType Type() const { return type_; }
    const glm::vec3& Extents() const { return extents_; }
    const glm::vec3& Offset() const { return offset_; }

    static std::shared_ptr<ZCollider> Create(const std::string& type, const glm::vec3& extents, const glm::vec3& offset = glm::vec3(0.f));

protected:

    void* ptr_ = nullptr;
    ZColliderType type_ = ZColliderType::None;
    glm::vec3 extents_;
    glm::vec3 offset_;

};
