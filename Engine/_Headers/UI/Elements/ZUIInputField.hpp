/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZUIInputField.hpp

    Created by Adrian Sanchez on 02/07/2021.
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
class ZUIPanel;
class ZUIText;
class ZInputKeyEvent;
class ZInputButtonEvent;

// Class and Data Structure Definitions
class ZUIInputField : public ZUIElement
{

    friend class ZUI;

public:

    ZUIInputField(const std::string& label = "", const std::string& placeholder = "", const glm::vec2& position = glm::vec2(0.5f), const glm::vec2& scale = glm::vec2(0.1f));
    ZUIInputField(const ZUIElementOptions& options, const std::string& label = "", const std::string& placeholder = "");
    ~ZUIInputField() {}

    void Initialize() override;
    void Initialize(const std::shared_ptr<ZOFNode>& root) override;

    const std::string& Placeholder() const { return placeholder_; }
    const std::string& Label() const { return label_; }
    const std::string& Text() const { return text_; }
    bool Focused() const { return focused_; }

    void SetPlaceholder(const std::string& placeholder) { placeholder_ = placeholder; }
    void SetLabel(const std::string& label);
    void SetText(const std::string& text);
    void SetFocused(bool focused);

    DECLARE_UI_CREATORS(ZUIInputField)

protected:

    std::string placeholder_;
    std::string label_;
    std::string text_;
    bool focused_ = false;

    std::shared_ptr<ZUIPanel> field_;
    std::shared_ptr<ZUIText> fieldText_;
    std::shared_ptr<ZUIText> labelText_;

    void CreateField();
    void CreateLabelField(const std::string& text);
    void DestroyLabelField();

    void ProcessKey(const ZKey& key);

    void HandleKeyPressed(const std::shared_ptr<ZInputKeyEvent>& event);
    void HandleButtonPressed(const std::shared_ptr<ZInputButtonEvent>& event);

};
