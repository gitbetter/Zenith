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

  using ZUICreator = std::shared_ptr<ZUIElement> (ZUIFactory::*)();

private:

public:

  ZUIFactory();
  ~ZUIFactory() { }

  ZUIElementMap Create(ZOFTree* data);

  std::shared_ptr<ZUIElement> CreateUIButton();
  std::shared_ptr<ZUIElement> CreateUIImage();
  std::shared_ptr<ZUIElement> CreateUIPanel();
  std::shared_ptr<ZUIElement> CreateUIText();

protected:

  std::map<std::string, ZUICreator> elementCreators_;

};
