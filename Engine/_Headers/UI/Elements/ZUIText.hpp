/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZUIText.hpp

    Created by Adrian Sanchez on 11/02/2019.
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

class ZUIText : public ZUIElement
{

public:

    ZUIText();
    ~ZUIText() = default;

    void OnInitialize() override;
    void OnDeserialize(const std::shared_ptr<ZOFObjectNode>& dataNode) override;
    
    void OnPrepare(double deltaTime, unsigned int zOrder = 0) override;

    const std::string& Text() const { return text_; }
    ZHFont Font();
    float FontScale() const { return fontScale_; }
    float LineSpacing() const { return lineSpacing_; }
    bool ShouldWrap() const { return wrapToBounds_; }
    ZAlignment HorizontalAlignment() const { return hAlignment_; }
    ZAlignment VerticalAlignment() const { return vAlignment_; }
    float MaxWrapBounds() const;

    void SetText(const std::string& text);
    void SetFontScale(float scale);
    void SetFont(const std::string& font);
    void SetWrap(bool wrap);
    void SetLineSpacing(float spacing);
    void SetHorizontalAlignment(ZAlignment alignment);
    void SetVerticalAlignment(ZAlignment alignment);

    DECLARE_UI_CREATORS(ZUIText)

protected:

    std::string                              text_;
    std::string                              fontName_;
    float                                    fontScale_;
    float                                    lineSpacing_;
    bool                                     wrapToBounds_;
    ZAlignment                               hAlignment_;
    ZAlignment                               vAlignment_;
    ZHFont                                   font_;
    std::shared_ptr<class ZVertexBuffer>     bufferData_;
    ZVertex2DDataOptions                     textVertexData_;

    void OnRectChanged() override;

private:

    void RecalculateBufferData();

};
