//
//  ZUIFactory.cpp
//  Zenith
//
//  Created by Adrian Sanchez on 10/03/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#include "ZUIFactory.hpp"

ZUIFactory::ZUIFactory() {
  elementCreators_["Button"] = &ZUIFactory::CreateUIButton;
  elementCreators_["Image"] = &ZUIFactory::CreateUIImage;
  elementCreators_["Panel"] = &ZUIFactory::CreateUIPanel;
  elementCreators_["Text"] = &ZUIFactory::CreateUIText;
}

ZUIElementMap ZUIFactory::Create(ZOFTree* data) {
  ZUIElementMap uiElements;
  for (ZOFChildMap::iterator it = data->children.begin(); it != data->children.end(); it++) {
    ZOFNode* node = it->second;
    if (node->id.find("ZUI") == 0) {

      // TODO: call corresponding creator based on type

    }
  }
  return uiElements;
}

std::shared_ptr<ZUIElement> ZUIFactory::CreateUIButton() {

}

std::shared_ptr<ZUIElement> ZUIFactory::CreateUIImage() {

}

std::shared_ptr<ZUIElement> ZUIFactory::CreateUIPanel() {

}

std::shared_ptr<ZUIElement> ZUIFactory::CreateUIText() {

}
