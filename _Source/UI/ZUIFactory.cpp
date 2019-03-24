/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZUIFactory.cpp

    Created by Adrian Sanchez on 10/03/2019.
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

#include "ZUIFactory.hpp"
#include "ZUIButton.hpp"
#include "ZUIImage.hpp"
#include "ZUIPanel.hpp"
#include "ZUIText.hpp"
#include "ZUICursor.hpp"
#include "ZUICheckBox.hpp"
#include "ZUIListPanel.hpp"

ZUIFactory::ZUIFactory() {
  elementCreators_["Button"] = &ZUIFactory::CreateUIButton;
  elementCreators_["Image"] = &ZUIFactory::CreateUIImage;
  elementCreators_["Panel"] = &ZUIFactory::CreateUIPanel;
  elementCreators_["Text"] = &ZUIFactory::CreateUIText;
  elementCreators_["Cursor"] = &ZUIFactory::CreateUICursor;
  elementCreators_["Checkbox"] = &ZUIFactory::CreateUICheckbox;
  elementCreators_["ListPanel"] = &ZUIFactory::CreateUIListPanel;
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
        if (elementCreators_.find(typeProp->value) == elementCreators_.end()) {
          _Z("Could not create a UI component of type " + typeProp->value, ZERROR); continue;
        }
        element = std::shared_ptr<ZUIElement>((this->*elementCreators_[typeProp->value])(uiNode));
      }

      // Recursively create children if there are any nested UI nodes
      if (element) {
        ZUIElementMap uiChildren = Create(uiNode);
        for (ZUIElementMap::iterator it = uiChildren.begin(); it != uiChildren.end(); it++) {
          element->AddChild(it->second);
        }

        uiElements.insert(uiChildren.begin(), uiChildren.end());
        uiElements[uiNode->id] = element;
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

std::shared_ptr<ZUIElement> ZUIFactory::CreateUICheckbox(ZOFNode* root) {
  std::shared_ptr<ZUICheckBox> checkbox = std::make_shared<ZUICheckBox>();
  checkbox->Initialize(root);
  return checkbox;
}

std::shared_ptr<ZUIElement> ZUIFactory::CreateUIListPanel(ZOFNode* root) {
  std::shared_ptr<ZUIListPanel> list = std::make_shared<ZUIListPanel>();
  list->Initialize(root);
  return list;
}
