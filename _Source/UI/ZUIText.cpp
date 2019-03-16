//
//  ZUIText.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 11/02/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#include "ZUIText.hpp"
#include "ZUI.hpp"
#include "ZGraphics.hpp"
#include "ZGraphicsStrategy.hpp"
#include "ZTextStrategy.hpp"
#include "ZEngine.hpp"
#include "ZDomain.hpp"

ZUIText::ZUIText(std::string text, std::string font, float fontSize, glm::vec2 position, glm::vec2 scale)
: ZUIElement(position, scale) {
  font_ = font;
  text_ = text;
  fontScale_ = fontSize;
  bufferData_ = ZEngine::Graphics()->Strategy()->LoadEmptyVertexData2D(4);
}

void ZUIText::Initialize(ZOFNode* root) {
  ZUIElement::Initialize(root);

  ZOFObjectNode* node = dynamic_cast<ZOFObjectNode*>(root);
  if(node == nullptr) {
    _Z("Could not initalize ZUIElement", ZERROR);
    return;
  }

  ZOFPropertyMap props = node->properties;

  if (props.find("font") != props.end() && props["font"]->HasValues()) {
    ZOFString* fontProp = props["font"]->Value<ZOFString>(0);
    font_ = fontProp->value;
    if (props["font"]->ValueCount() > 1) {
      ZOFNumber* fontSizeProp = props["font"]->Value<ZOFNumber>(1);
      fontScale_ = fontSizeProp->value;
    }
  }

  if (props.find("text") != props.end() && props["text"]->HasValues()) {
    ZOFString* textProp = props["text"]->Value<ZOFString>(0);
    text_ = textProp->value;
  }
}

void ZUIText::Draw(ZShader* shader) {
  ZEngine::Graphics()->Strategy()->EnableAlphaBlending();
      // TODO: Add text alignment property that calculates these value accordingly
  float x = Position().x,
        y = Position().y;
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
    ZEngine::Graphics()->Strategy()->UpdateBuffer(bufferData_, vertices);
    ZEngine::Graphics()->Strategy()->Draw(bufferData_, vertices);

    x += (character.advance >> 6) * fontScale_;
  }
  ZEngine::Graphics()->Strategy()->DisableAlphaBlending();
  RenderChildren(shader);
}
