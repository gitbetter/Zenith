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
#include "ZInput.hpp"
#include "ZUIText.hpp"
#include "ZGLTextStrategy.hpp"
#include "ZDomain.hpp"

void ZUI::Initialize()
{
// TODO: Switch the strategies here based on implementation details
    if (textStrategy_ == nullptr)
    {
        textStrategy_.reset(new ZGLTextStrategy);
        textStrategy_->Initialize();
    }

    if (uiShader_ == nullptr)
    {
        uiShader_ = std::shared_ptr<ZShader>(new ZShader("/Shaders/Vertex/ui.vert", "/Shaders/Pixel/ui.frag"));
        uiShader_->Initialize();
    }

    if (textShader_ == nullptr)
    {
        textShader_ = std::shared_ptr<ZShader>(new ZShader("/Shaders/Vertex/text.vert", "/Shaders/Pixel/text.frag"));
        textShader_->Initialize();
    }
}

void ZUI::RegisterFont(std::string fontPath)
{
    if (textStrategy_ != nullptr) textStrategy_->LoadFontAsync(fontPath, 64);
}

void ZUI::Render(ZUIElementMap elements)
{
    zenith::Graphics()->Strategy()->EnableAlphaBlending();
    for (ZUIElementMap::iterator it = elements.begin(); it != elements.end(); it++)
    {
        // Only render the top level elements that are not hidden. The children will
        // be rendered within the respective parent elements.
        RenderElement(it->second);
    }
    zenith::Graphics()->Strategy()->DisableAlphaBlending();
}

void ZUI::RenderElement(std::shared_ptr<ZUIElement> element)
{
    // TODO: Also only render if the child has the dirty flag set
    if (element->hidden_) return;

    switch (element->type_)
    {
    case ZUIElementType::Image:
        RenderImage(element);
        break;
    case ZUIElementType::Text:
        RenderText(element);
        break;
    case ZUIElementType::Button:
    case ZUIElementType::CheckBox:
    case ZUIElementType::ListPanel:
    case ZUIElementType::Panel:
    case ZUIElementType::Unknown:
        RenderGeneric(element);
        break;
    }

    for (std::shared_ptr<ZUIElement> child : element->children_)
    {
        RenderElement(child);
    }
}

void ZUI::RenderGeneric(std::shared_ptr<ZUIElement>& element)
{
    std::shared_ptr<ZMesh2D> mesh = element->ElementShape();
    element->shader_->Activate();

    zenith::Graphics()->Strategy()->BindTexture(element->texture_, 0);
    element->shader_->SetInt(element->texture_.type + "0", 0);

    glm::mat4 ortho = glm::ortho(0.f, (float) zenith::Domain()->ResolutionX(), (float) zenith::Domain()->ResolutionY(), 0.f);
    element->shader_->SetMat4("M", element->modelMatrix_);
    element->shader_->SetMat4("P", ortho);
    element->shader_->SetVec4("color", element->color_);
    element->shader_->SetVec4("borderColor", glm::vec4(0.f));
    element->shader_->SetFloat("borderWidth", 0.f);
    element->shader_->SetFloat("aspectRatio", 1.f);

    if (element->border_.width > 0.f)
    {
        float borderWidth = element->border_.width / glm::length(element->Size());
        float aspect = element->Size().y / element->Size().x;
        element->shader_->SetVec4("borderColor", element->border_.color);
        element->shader_->SetFloat("borderWidth", borderWidth);
        element->shader_->SetFloat("aspectRatio", aspect);
    }

    mesh->Render(element->shader_.get());
}

void ZUI::RenderImage(std::shared_ptr<ZUIElement>& element)
{
    RenderGeneric(element);
}

void ZUI::RenderText(std::shared_ptr<ZUIElement>& element)
{
    auto textEl = std::dynamic_pointer_cast<ZUIText>(element);
    // TODO: Add text alignment property that calculates these value accordingly
    float x = textEl->Position().x,
        y = textEl->Position().y,
        xRatio = zenith::Domain()->ResolutionXRatio(),
        yRatio = zenith::Domain()->ResolutionYRatio();
    for (auto c = textEl->text_.begin(); c != textEl->text_.end(); c++)
    {
        ZCharacter character = zenith::UI()->TextStrategy()->Character(textEl->font_, *c);
        textEl->texture_ = character.texture;

        float xpos = x + character.bearing.x * textEl->fontScale_ * xRatio;
        float ypos = y - (character.size.y - character.bearing.y) * textEl->fontScale_ * yRatio;
        float w = character.size.x * textEl->fontScale_ * xRatio;
        float h = character.size.y * textEl->fontScale_ * yRatio;

        std::vector<ZVertex2D> vertices = {
          ZVertex2D(glm::vec2(xpos, ypos), glm::vec2(0.f, 0.f)),
          ZVertex2D(glm::vec2(xpos, ypos + h), glm::vec2(0.f, 1.f)),
          ZVertex2D(glm::vec2(xpos + w, ypos), glm::vec2(1.f, 0.f)),
          ZVertex2D(glm::vec2(xpos + w, ypos + h), glm::vec2(1.f, 1.f))
        };

        RenderGeneric(element);

        zenith::Graphics()->Strategy()->UpdateBuffer(textEl->bufferData_, vertices);
        zenith::Graphics()->Strategy()->Draw(textEl->bufferData_, vertices);

        x += (character.advance >> 6) * textEl->fontScale_ * xRatio;
    }
}

void ZUI::CleanUp()
{
    if (textShader_ != nullptr)
    {
        textShader_.reset();
    }

    if (uiShader_ != nullptr)
    {
        uiShader_.reset();
    }

    textStrategy_.reset();
}
