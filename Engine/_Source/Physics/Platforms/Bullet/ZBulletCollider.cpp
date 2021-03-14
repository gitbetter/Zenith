/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZBulletCollider.cpp

    Created by Adrian Sanchez on 02/20/2021.
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

#include "ZBulletCollider.hpp"
#include "btBulletDynamicsCommon.h"

void ZBulletCollider::Initialize()
{
    switch (type_) {
    case ZColliderType::Box:
        ptr_ = new btBoxShape(btVector3(static_cast<double>(extents_.x), static_cast<double>(extents_.y), static_cast<double>(extents_.z)));
        break;
    case ZColliderType::Sphere:
        ptr_ = new btSphereShape(static_cast<double>(extents_.x));
        break;
    case ZColliderType::Capsule:
        ptr_ = new btCapsuleShape(static_cast<double>(extents_.x), static_cast<double>(extents_.y));
        break;
    case ZColliderType::Cylinder:
        ptr_ = new btCylinderShape(btVector3(static_cast<double>(extents_.x), static_cast<double>(extents_.y), static_cast<double>(extents_.z)));
        break;
    case ZColliderType::Cone:
        ptr_ = new btConeShape(static_cast<double>(extents_.x), static_cast<double>(extents_.y));
        break;
    case ZColliderType::None:
        ptr_ = new btEmptyShape();
    default: break;
    }
}
