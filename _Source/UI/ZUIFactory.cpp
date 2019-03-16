//
//  ZUIFactory.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 10/03/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#include "ZUIFactory.hpp"
#include "ZUIButton.hpp"
#include "ZUIImage.hpp"
#include "ZUIPanel.hpp"
#include "ZUIText.hpp"
#include "ZUICursor.hpp"

ZUIFactory::ZUIFactory() {
  elementCreators_["Button"] = &ZUIFactory::CreateUIButton;
  elementCreators_["Image"] = &ZUIFactory::CreateUIImage;
  elementCreators_["Panel"] = &ZUIFactory::CreateUIPanel;
  elementCreators_["Text"] = &ZUIFactory::CreateUIText;
  elementCreators_["Cursor"] = &ZUIFactory::CreateUICursor;
}

ZUIElementMap ZUIFactory::Create(ZOFTree* data) {
  ZUIElementMap uiElements;
  for (ZOFChildMap::iterator it = data->children.begin(); it != data->children.end(); it++) {
    ZOFNode* node = it->second;
    if (node->id.find("ZUI") == 0) {
      ZOFObjectNode* uiNode = dynamic_cast<ZOFObjectNode*>(node);
      ZOFPropertyMap props = uiNode->properties;

      std::shared_ptr<ZUIElement> element;

      if (props.find("type") != props.end() && props["type"]->HasValues()) {
        ZOFString* typeProp = props["type"]->Value<ZOFString>(0);
        element = std::shared_ptr<ZUIElement>((this->*elementCreators_[typeProp->value])(uiNode));
        uiElements[uiNode->id] = element;
      }

      // Recursively create children if there are any nested UI nodes
      if (element) {
        ZUIElementMap uiChildren = Create(uiNode);
        for (ZUIElementMap::iterator it = uiChildren.begin(); it != uiChildren.end(); it++) {
          element->AddChild(it->second);
        }
      }
    }
  }
  return uiElements;
}

std::shared_ptr<ZUIElement> ZUIFactory::CreateUIButton(ZOFNode* root) {
  std::shared_ptr<ZUIButton> button = std::make_shared<ZUIButton>();
  button->Initialize(root);
  return button;
}

std::shared_ptr<ZUIElement> ZUIFactory::CreateUIImage(ZOFNode* root) {
  std::shared_ptr<ZUIImage> image = std::make_shared<ZUIImage>();
  image->Initialize(root);
  return image;
}

std::shared_ptr<ZUIElement> ZUIFactory::CreateUIPanel(ZOFNode* root) {
  std::shared_ptr<ZUIPanel> panel = std::make_shared<ZUIPanel>();
  panel->Initialize(root);
  return panel;
}

std::shared_ptr<ZUIElement> ZUIFactory::CreateUIText(ZOFNode* root) {
  std::shared_ptr<ZUIText> text = std::make_shared<ZUIText>();
  text->Initialize(root);
  return text;
}

std::shared_ptr<ZUIElement> ZUIFactory::CreateUICursor(ZOFNode* root) {
  std::shared_ptr<ZUICursor> cursor = std::make_shared<ZUICursor>();
  cursor->Initialize(root);
  return cursor;
}
