/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZUIText.cpp

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

#include "ZUIText.hpp"
#include "ZUI.hpp"
#include "ZGraphics.hpp"
#include "ZGraphicsStrategy.hpp"
#include "ZTextStrategy.hpp"
#include "ZEngine.hpp"
#include "ZShader.hpp"
#include "ZDomain.hpp"

ZUIText::ZUIText(std::string text, std::string font, float fontSize, glm::vec2 position, glm::vec2 scale)
: ZUIElement(position, scale) {
  font_ = font;
  text_ = text;
  fontScale_ = fontSize;
  enabled_ = false;
  bufferData_ = ZEngine::Graphics()->Strategy()->LoadEmptyVertexData2D(4);
}

void ZUIText::Initialize(std::shared_ptr<ZOFNode> root) {
  ZUIElement::Initialize(root);

  std::shared_ptr<ZOFObjectNode> node = std::dynamic_pointer_cast<ZOFObjectNode>(root);
  if(node == nullptr) {
    _Z("Could not initalize ZUIElement", ZERROR);
    return;
  }

  ZOFPropertyMap props = node->properties;

  if (props.find("font") != props.end() && props["font"]->HasValues()) {
    std::shared_ptr<ZOFString> fontProp = props["font"]->Value<ZOFString>(0);
    font_ = fontProp->value;
    if (props["font"]->ValueCount() > 1) {
      std::shared_ptr<ZOFNumber> fontSizeProp = props["font"]->Value<ZOFNumber>(1);
      fontScale_ = fontSizeProp->value;
    }
  }

  if (props.find("text") != props.end() && props["text"]->HasValues()) {
    std::shared_ptr<ZOFString> textProp = props["text"]->Value<ZOFString>(0);
    text_ = textProp->value;
  }
}

void ZUIText::Render(ZRenderOp renderOp) {
  ZEngine::Graphics()->Strategy()->EnableAlphaBlending();
      // TODO: Add text alignment property that calculates these value accordingly
  float x = Position().x,
        y = Position().y,
        xRatio = ZEngine::Domain()->ResolutionXRatio(),
        yRatio = ZEngine::Domain()->ResolutionYRatio();
  for (auto c = text_.begin(); c != text_.end(); c++) {
    ZCharacter character = ZEngine::UI()->TextStrategy()->Character(font_, *c);
    texture_ = character.texture;

    float xpos = x + character.bearing.x * fontScale_;
    float ypos = y - (character.size.y - character.bearing.y) * fontScale_;
    float w = character.size.x * fontScale_ * xRatio;
    float h = character.size.y * fontScale_ * yRatio;

    std::vector<ZVertex2D> vertices = {
      ZVertex2D(glm::vec2(xpos, ypos), glm::vec2(0.f, 0.f)),
      ZVertex2D(glm::vec2(xpos, ypos + h), glm::vec2(0.f, 1.f)),
      ZVertex2D(glm::vec2(xpos + w, ypos), glm::vec2(1.f, 0.f)),
      ZVertex2D(glm::vec2(xpos + w, ypos + h), glm::vec2(1.f, 1.f))
    };

    ZUIElement::Render();

    ZEngine::Graphics()->Strategy()->UpdateBuffer(bufferData_, vertices);
    ZEngine::Graphics()->Strategy()->Draw(bufferData_, vertices);

    x += (character.advance >> 6) * fontScale_ * xRatio;
  }
  ZEngine::Graphics()->Strategy()->DisableAlphaBlending();
  RenderChildren();
}
