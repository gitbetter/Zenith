/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZUILabeledElement.hpp

    Created by Adrian Sanchez on 02/16/2021.
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
#include "ZUIElement.hpp"

// Forward Declarations
class ZScene;
class ZUIText;

// Class and Data Structure Definitions
class ZUILabeledElement : public ZUIElement
{

public:

    ZUILabeledElement(const std::string& label, const std::shared_ptr<ZUIElement>& element);
    ~ZUILabeledElement() {}

    void                                        Initialize();

    void                                        Initialize(const std::shared_ptr<ZOFNode>& root);

    const std::shared_ptr<ZUIText>              LabelField() const { return labelText_; }
    const std::shared_ptr<ZUIElement>           Element() const { return element_; }

    void                                        SetLabel(const std::string& label);
    void                                        SetLabelWidth(float width);
    void                                        SetLabelTextColor(const glm::vec4& color);
    void                                        SetLabelFontSize(float size);
    void                                        SetLabelTextAlignment(ZAlignment alignment);

    static std::shared_ptr<ZUILabeledElement>   Create(const std::string& label, const std::shared_ptr<ZUIElement>& element);

protected:

    std::string                             label_;
    glm::vec4                               labelTextColor_ = glm::vec4(1.f);
    float                                   labelWidth_ = 0.3f;
    float                                   labelFontSize_ = 14.f;

    std::shared_ptr<ZUIText>                labelText_;
    std::shared_ptr<ZUIElement>             element_;

    void                                        CreateLabelField();
    void                                        SetupElement();

};
