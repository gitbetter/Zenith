/*

   ______     ______     __   __     __     ______   __  __    
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \   
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \  
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\ 
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/ 
                                                          
    ZUI.cpp

    Created by Adrian Sanchez on 06/02/2019.
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

#include "ZUI.hpp"
#include "ZShader.hpp"
#include "ZUICursor.hpp"
#include "ZEngine.hpp"
#include "ZInput.hpp"
#include "ZUIText.hpp"
#include "ZGLTextStrategy.hpp"

void ZUI::Initialize() {
  // TODO: Switch the strategies here based on implementation details
  if(textStrategy_ == nullptr) {
    textStrategy_.reset(new ZGLTextStrategy);
    textStrategy_->Initialize();
  }

  if (uiShader_ == nullptr) {
    uiShader_ = std::shared_ptr<ZShader>(new ZShader);
    uiShader_->Initialize("Assets/Shaders/Vertex/ui.vert", "Assets/Shaders/Pixel/ui.frag");
  }

  if (textShader_ == nullptr) {
    textShader_ = std::shared_ptr<ZShader>(new ZShader);
    textShader_->Initialize("Assets/Shaders/Vertex/text.vert", "Assets/Shaders/Pixel/text.frag");
  }
}

void ZUI::Draw() {
  for (ZUIElementMap::iterator it = elements_.begin(); it != elements_.end(); it++) {
    // Only render the top level elements that are not hidden. The children will
    // be rendered within the respective parent elements.
    if (!it->second->Hidden() && !it->second->Parent()) it->second->Render();
  }
  if (cursor_ != nullptr) cursor_->Render();
}

void ZUI::SetCursor(std::shared_ptr<ZUICursor> cursor) {
  cursor_ = cursor;
  cursor_->SetShader(uiShader_);
}

void ZUI::AddElement(std::shared_ptr<ZUIElement> element) {
  if (element != nullptr) {
    if (std::dynamic_pointer_cast<ZUIText>(element)) element->SetShader(textShader_);
    else element->SetShader(uiShader_);
    elements_[element->ID()] = element;
  }
}

void ZUI::AddElements(std::initializer_list<std::shared_ptr<ZUIElement>> elements) {
  for (std::shared_ptr<ZUIElement> element : elements) {
    AddElement(element);
  }
}

void ZUI::RegisterFont(std::string fontPath) {
  if (textStrategy_ != nullptr) textStrategy_->LoadFont(fontPath, 64);
}

void ZUI::CleanUp() {
  for (ZUIElementMap::iterator it = elements_.begin(); it != elements_.end(); it++) {
    it->second->CleanUp();
  }
  elements_.clear();

  if (cursor_ != nullptr) {
    cursor_->CleanUp(); cursor_.reset();
  }

  if (uiShader_ != nullptr) {
    uiShader_.reset();
  }

  textStrategy_.reset();

  if (textShader_ != nullptr) {
    textShader_.reset();
  }
}
