//
//  ZUIFactory.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 10/03/2019.
//  Copyright © 2019 Pervasive Sense. All rights reserved.
//

#pragma once

// Includes
#include "ZCommon.hpp"
#include "ZUIElement.hpp"

// Forward Declarations
//class SomeClass;

// Class and Data Structure Definitions
class ZUIFactory {

  using ZUICreator = std::shared_ptr<ZUIElement> (ZUIFactory::*)(ZOFNode* root);

private:

public:

  ZUIFactory();
  ~ZUIFactory() { }

  ZUIElementMap Create(ZOFTree* data);

  std::shared_ptr<ZUIElement> CreateUIButton(ZOFNode* root);
  std::shared_ptr<ZUIElement> CreateUIImage(ZOFNode* root);
  std::shared_ptr<ZUIElement> CreateUIPanel(ZOFNode* root);
  std::shared_ptr<ZUIElement> CreateUIText(ZOFNode* root);
  std::shared_ptr<ZUIElement> CreateUICursor(ZOFNode* root);
  std::shared_ptr<ZUIElement> CreateUICheckbox(ZOFNode* root);

protected:

  std::map<std::string, ZUICreator> elementCreators_;

};
