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

// Includes
#include "ZUIElement.hpp"

// Forward Declarations
class ZBuffer;
class ZFont;

// Class and Data Structure Definitions
class ZUIText : public ZUIElement
{

    friend class ZUI;

public:

    ZUIText(const std::string& text = "", const std::string& font = "", float fontSize = 1.f, const glm::vec2& position = glm::vec2(0.5f), const glm::vec2& scale = glm::vec2(0.1f));
    ZUIText(const ZUIElementOptions& options, const std::string& text = "", const std::string& font = "", float fontSize = 1.f);
    ~ZUIText() {}

    void Initialize() override;
    void Initialize(const std::shared_ptr<ZOFNode>& root) override;

    const std::string& Text() const { return text_; }
    std::shared_ptr<ZFont> Font();
    float FontScale() const { return fontScale_; }
    float LineSpacing() const { return lineSpacing_; }
    bool ShouldWrap() const { return wrapToBounds_; }
    float MaxWrapBounds() const;

    void SetText(const std::string& text);
    void SetFontScale(float scale);
    void SetFont(const std::string& font);
    void SetWrap(bool wrap);
    void SetLineSpacing(float spacing);

    DECLARE_UI_CREATORS(ZUIText)

protected:

    std::string text_;
    std::string fontName_;
    float fontScale_;
    float lineSpacing_;
    bool wrapToBounds_;
    std::shared_ptr<ZFont> font_;
    std::shared_ptr<ZBuffer> bufferData_;
    ZVertex2DDataOptions textVertexData_;

    void Draw() override;

    void OnRectChanged() override;

private:

    void RecalculateBufferData();

};
