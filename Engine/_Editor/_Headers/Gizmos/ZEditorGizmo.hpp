/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZEditorGizmo.hpp

    Created by Adrian Sanchez on 02/20/21.
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

#include "ZEditorEntity.hpp"

class ZRay;

class ZEditorGizmo : public ZEditorEntity
{
public:

    ZEditorGizmo() { }
    virtual ~ZEditorGizmo() { }

    virtual void Initialize(const std::shared_ptr<ZScene>& scene) override;

    void Update() override { }

    bool Showing();
    void SetPosition(const glm::vec3& position);
    void Hide();
    void Show();

    virtual bool TryActivate(const ZRect& viewportRect) = 0;
    virtual void Manipulate(const ZRect& viewportRect, glm::mat4& transform) = 0;
    virtual void Deactivate() = 0;

protected:

    ZHGameObject gizmo_;
    bool active_ = false;

};
