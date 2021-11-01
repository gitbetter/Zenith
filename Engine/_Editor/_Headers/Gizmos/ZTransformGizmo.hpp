/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZTransformGizmo.hpp

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

#include "ZEditorGizmo.hpp"

class ZScene;
class ZGameObject;

class ZTransformGizmo : public ZEditorGizmo
{
public:

    ZTransformGizmo() 
        : ZEditorGizmo()
    { }
    virtual ~ZTransformGizmo() { }

    void Initialize(const std::shared_ptr<ZScene>& scene) override;

    void Update() override;

    void CleanUp() override;

    bool TryActivate(const ZRect& viewportRect) override;

    void Manipulate(const ZRect& viewportRect, glm::mat4& transform) override;

    void Deactivate() override;

protected:

    std::shared_ptr<ZScene> previousActiveProjectScene_ = nullptr;
    std::array<ZHGameObject, 3> axisArrows_;
    int selectedAxis_ = -1;
    glm::vec3 previousAxisPoint_{ 0.f };
    glm::vec2 previousCursorPos_{ 0.f };
    float sensitivity_ = 0.3f;

    void OnProjectSceneChanged() override;

    ZHGameObject CreateAxisArrow(const glm::vec3& axis);

};
