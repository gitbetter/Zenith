/*
 
  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/
 
    ZGameObjectControls.hpp
 
    Created by Adrian Sanchez on 03/13/21.
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
#include "ZEditorControl.hpp"

 // Forward Declarations
class ZGameObject;
class ZUIPanel;
class ZTextField;
class ZBoolField;
class ZVec3Field;

// Definitions
class ZGameObjectControls : public ZEditorControl {

public:

    ZGameObjectControls(const std::shared_ptr<ZGameObject>& object, const ZUITheme& theme = ZUITheme())
        : ZEditorControl(theme), gameObject_(object)
    { }
    ~ZGameObjectControls() {}

    void Initialize(const std::shared_ptr<ZScene>& scene) override;
    void Update() override;
    void CleanUp() override { };

    bool Active() const { return gameObject_ != nullptr; }
    std::shared_ptr<ZUIPanel> Header() const { return objectHeader_; }
    std::shared_ptr<ZUIPanel> TransformFields() const { return transformFields_; }

    void SetGameObject(const std::shared_ptr<ZGameObject>& gameObject) { gameObject_ = gameObject; }

    static std::shared_ptr<ZGameObjectControls> Create(const std::shared_ptr<ZGameObject>& object, const std::shared_ptr<ZScene>& scene = nullptr, ZUITheme theme = ZUITheme());

protected:

    std::shared_ptr<ZGameObject> gameObject_ = nullptr;

    std::shared_ptr<ZUIPanel> objectHeader_ = nullptr;
    std::shared_ptr<ZUIPanel> transformFields_ = nullptr;

    std::shared_ptr<ZTextField> nameField_ = nullptr;
    std::shared_ptr<ZBoolField> activeField_ = nullptr;
    std::shared_ptr<ZVec3Field> positionField_ = nullptr;
    std::shared_ptr<ZVec3Field> rotationField_ = nullptr;
    std::shared_ptr<ZVec3Field> scaleField_ = nullptr;

    void SetupObjectHeader(const std::shared_ptr<ZScene>& scene);
    void SetupObjectHeaderFields(const std::shared_ptr<ZScene>& scene);
    void SetupTransformFields(const std::shared_ptr<ZScene>& scene);

};