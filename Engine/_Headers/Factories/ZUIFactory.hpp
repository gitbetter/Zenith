/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZUIFactory.hpp

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

#pragma once

// Includes
#include "ZCommon.hpp"
#include "ZUIElement.hpp"

// Forward Declarations
//class SomeClass;

// Class and Data Structure Definitions
class ZUIFactory {

  using ZUICreator = std::shared_ptr<ZUIElement> (ZUIFactory::*)(std::shared_ptr<ZOFNode> root);

private:

public:

  ZUIFactory();
  ~ZUIFactory() { }

  ZUIElementMap Load(std::shared_ptr<ZOFTree> data);

  std::shared_ptr<ZUIElement> CreateUIButton(std::shared_ptr<ZOFNode> root);
  std::shared_ptr<ZUIElement> CreateUIImage(std::shared_ptr<ZOFNode> root);
  std::shared_ptr<ZUIElement> CreateUIPanel(std::shared_ptr<ZOFNode> root);
  std::shared_ptr<ZUIElement> CreateUIText(std::shared_ptr<ZOFNode> root);
  std::shared_ptr<ZUIElement> CreateUICursor(std::shared_ptr<ZOFNode> root);
  std::shared_ptr<ZUIElement> CreateUICheckbox(std::shared_ptr<ZOFNode> root);
  std::shared_ptr<ZUIElement> CreateUIListPanel(std::shared_ptr<ZOFNode> root);

protected:

  std::map<std::string, ZUICreator> elementCreators_;

};
