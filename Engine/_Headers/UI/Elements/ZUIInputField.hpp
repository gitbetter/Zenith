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

#include "ZUIElement.hpp"

class ZUIInputField : public ZUIElement
{

    friend class ZUI;

public:

    ZUIInputField();
    ~ZUIInputField() = default;

    virtual void                                OnInitialize() override;
    virtual void                                OnDeserialize(const std::shared_ptr<ZOFObjectNode>& dataNode) override;

    const std::string&                          Placeholder() const { return placeholder_; }
    const std::string&                          Text() const { return text_; }
    const glm::vec4&                            TextColor() const { return textColor_; }
    const glm::vec2&                            FieldPadding() const { return fieldPadding_; }
    bool                                        Focused() const { return focused_; }
    const ZHUIElement                           TextField() const { return fieldText_; }

    void                                        SetPlaceholder(const std::string& placeholder) { placeholder_ = placeholder; }
    void                                        SetText(const std::string& text);
    void                                        SetTextColor(const glm::vec4& color);
    void                                        SetHighlightBorder(const ZUIBorder& border);
    void                                        SetFocused(bool focused);
    void                                        SetBackground(const ZHTexture& texture);
    void                                        SetBackground(const glm::vec4& color);
    void                                        SetFontSize(float size);
    void                                        SetFieldPadding(const glm::vec2& padding);
    void                                        SetCharacterFilter(const std::function<bool(char)>& filter) { filter_ = filter; }

    void                                        OnInputChanged(const std::function<void(const std::string&)>& cb) { onInputChangedCallback_ = cb; }

    DECLARE_UI_CREATORS(ZUIInputField)

protected:

    std::string                             placeholder_;
    std::string                             text_;
    glm::vec4                               textColor_;
    glm::vec2                               fieldPadding_;
    ZUIBorder                               highlightBorder_;
    float                                   fontSize_ = 14.f;
    bool                                    focused_ = false;
    std::function<bool(char)>               filter_ = nullptr;
    std::function<void(const std::string&)> onInputChangedCallback_ = nullptr;

    ZHUIElement                             fieldText_;

    void                                    CreateTextArea();

    void                                    ProcessKey(const ZKey& key);

    void                                    HandleKeyPressed(const std::shared_ptr<class ZInputKeyEvent>& event);
    void                                    HandleButtonPressed(const std::shared_ptr<class ZInputButtonEvent>& event);

};
