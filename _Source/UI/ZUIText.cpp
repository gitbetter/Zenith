//
//  ZUIText.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 11/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#include "ZUIText.hpp"
#include "ZUI.hpp"
#include "ZGraphicsStrategy.hpp"
#include "ZTextStrategy.hpp"
#include "ZEngine.hpp"
#include "ZDomain.hpp"

ZUIText::ZUIText(std::string text, std::string font, float fontSize, glm::vec2 position, glm::vec2 scale)
: ZUIElement(position, scale) {
  font_ = font;
  text_ = text;
  fontScale_ = fontSize;
  bufferData_ = ZEngine::UI()->GraphicsStrategy()->LoadEmptyVertexData2D(4);
}

void ZUIText::Draw(ZShader* shader) {
  ZEngine::UI()->GraphicsStrategy()->EnableAlphaBlending();
      // TODO: Add text alignment property that calculates these value accordingly
  float x = Position().x * (float)ZEngine::Domain()->ResolutionX() / 2.f,
        y = (Position().y * (float)ZEngine::Domain()->ResolutionY()) - (Size().y * (float)ZEngine::Domain()->ResolutionY()) / 2.f;
  for (auto c = text_.begin(); c != text_.end(); c++) {
    ZCharacter character = ZEngine::UI()->TextStrategy()->Character(font_, *c);
    texture_ = character.texture;

    float xpos = x + character.bearing.x * fontScale_;
    float ypos = y - (character.size.y - character.bearing.y) * fontScale_;
    float w = character.size.x * fontScale_;
    float h = character.size.y * fontScale_;

    std::vector<ZVertex2D> vertices = {
      ZVertex2D(glm::vec2(xpos, ypos), glm::vec2(0.f, 0.f)),
      ZVertex2D(glm::vec2(xpos, ypos + h), glm::vec2(0.f, 1.f)),
      ZVertex2D(glm::vec2(xpos + w, ypos), glm::vec2(1.f, 0.f)),
      ZVertex2D(glm::vec2(xpos + w, ypos + h), glm::vec2(1.f, 1.f))
    };

    ZUIElement::Draw(shader);
    ZEngine::UI()->GraphicsStrategy()->UpdateBuffer(bufferData_, vertices);
    ZEngine::UI()->GraphicsStrategy()->Draw(bufferData_, vertices);

    x += (character.advance >> 6) * fontScale_;
  }
  ZEngine::UI()->GraphicsStrategy()->DisableAlphaBlending();
  RenderChildren(shader);
}
