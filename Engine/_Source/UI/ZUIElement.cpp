/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZUIElement.cpp

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

#include "ZUIElement.hpp"
#include "ZShader.hpp"
#include "ZUIText.hpp"
#include "ZDomain.hpp"
#include "ZUI.hpp"
#include "ZEventAgent.hpp"
#include "ZObjectSelectedEvent.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_interpolation.hpp>

ZUIElement::ZUIElement(glm::vec2 position, glm::vec2 scale) : modelMatrix_(1.0), color_(0.6)
{
    translationBounds_ = glm::vec4(0.f, (float) zenith::Domain()->ResolutionX(), 0.f, (float) zenith::Domain()->ResolutionY());
    SetPosition(position); SetSize(scale);
    enabled_ = true;
    hidden_ = false;
    id_ = "ZUI_" + zenith::IDSequence()->Next();
}

void ZUIElement::Initialize(std::shared_ptr<ZOFNode> root)
{
    modelMatrix_ = glm::mat4(1.0); color_ = glm::vec4(0.6);

    std::shared_ptr<ZOFObjectNode> node = std::static_pointer_cast<ZOFObjectNode>(root);
    if (node == nullptr)
    {
        zenith::Log("Could not initalize ZUIElement", ZSeverity::Error);
        return;
    }

    id_ = node->id;

    ZOFPropertyMap props = node->properties;

    glm::vec2 size = Size(), position = Position();

    if (props.find("scale") != props.end() && props["scale"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> scaleProp = props["scale"]->Value<ZOFNumberList>(0);
        float x = scaleProp->value[0] < 0 ? glm::min(zenith::Domain()->WindowWidth(), zenith::Domain()->ResolutionX()) : scaleProp->value[0] * zenith::Domain()->ResolutionXRatio();
        float y = scaleProp->value[1] < 0 ? glm::min(zenith::Domain()->WindowHeight(), zenith::Domain()->ResolutionY()) : scaleProp->value[1] * zenith::Domain()->ResolutionYRatio();
        size = glm::vec2(x, y);
    }

    if (props.find("position") != props.end() && props["position"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> posProp = props["position"]->Value<ZOFNumberList>(0);
        float x = posProp->value[0] * zenith::Domain()->ResolutionXRatio();
        float y = posProp->value[1] * zenith::Domain()->ResolutionYRatio();
        position = glm::vec2(x + size.x, y + size.y);
    }

    if (props.find("anchor") != props.end() && props["anchor"]->HasValues())
    {
        std::shared_ptr<ZOFString> vAnchorProp = props["anchor"]->Value<ZOFString>(0);
        std::shared_ptr<ZOFString> hAnchorProp = props["anchor"]->Value<ZOFString>(1);
        // TODO: A parent pointer is needed to determine the bounds and therefore the anchor relative positioning.
        // if there is no parent we simply anchor to the screen/resolution bounds
    }

    SetPosition(position);
    SetSize(size);

    if (props.find("color") != props.end() && props["color"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> colorProp = props["color"]->Value<ZOFNumberList>(0);
        color_ = glm::vec4(colorProp->value[0], colorProp->value[1], colorProp->value[2], colorProp->value[3]);
    }

    if (props.find("isHidden") != props.end() && props["isHidden"]->HasValues())
    {
        std::shared_ptr<ZOFString> hiddenProp = props["isHidden"]->Value<ZOFString>(0);
        hidden_ = hiddenProp->value == "Yes";
    }

    if (props.find("isEnabled") != props.end() && props["isEnabled"]->HasValues())
    {
        std::shared_ptr<ZOFString> enabledProp = props["isEnabled"]->Value<ZOFString>(0);
        enabled_ = enabledProp->value == "Yes";
    }

    if (props.find("texture") != props.end() && props["texture"]->HasValues())
    {
        std::shared_ptr<ZOFString> texProp = props["texture"]->Value<ZOFString>(0);
        if (zenith::Graphics()->Textures().find(texProp->value) != zenith::Graphics()->Textures().end())
            texture_ = zenith::Graphics()->Textures()[texProp->value];
    }

    if (props.find("borderWidth") != props.end() && props["borderWidth"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> borderWidthProp = props["borderWidth"]->Value<ZOFNumber>(0);
        border_.width = borderWidthProp->value;
    }

    if (props.find("borderColor") != props.end() && props["borderColor"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> borderColorProp = props["borderColor"]->Value<ZOFNumberList>(0);
        border_.color = glm::vec4(borderColorProp->value[0], borderColorProp->value[1], borderColorProp->value[2], borderColorProp->value[3]);
    }
}

void ZUIElement::Render(ZRenderOp renderOp)
{
    std::shared_ptr<ZMesh2D> mesh = ElementShape();
    shader_->Activate();

    zenith::Graphics()->Strategy()->BindTexture(texture_, 0);
    shader_->SetInt(texture_.type + "0", 0);

    glm::mat4 ortho = glm::ortho(0.f, (float) zenith::Domain()->ResolutionX(), (float) zenith::Domain()->ResolutionY(), 0.f);
    shader_->SetMat4("M", modelMatrix_);
    shader_->SetMat4("P", ortho);
    shader_->SetVec4("color", color_);
    shader_->SetVec4("borderColor", glm::vec4(0.f));
    shader_->SetFloat("borderWidth", 0.f);

    if (border_.width > 0.f)
    {
        float borderWidth = border_.width / glm::length(Size());
        float aspect = Size().y / Size().x;
        shader_->SetVec4("borderColor", border_.color);
        shader_->SetFloat("borderWidth", borderWidth);
        shader_->SetFloat("aspectRatio", aspect);
    }

    mesh->Render(shader_.get());
}

void ZUIElement::RenderChildren()
{
    for (std::shared_ptr<ZUIElement> child : children_)
    {
// TODO: Also only render if the child has the dirty flag set
        if (!child->Hidden()) child->Render();
    }
}

void ZUIElement::AddChild(std::shared_ptr<ZUIElement> element)
{
// Reset the child translation and move it to the parent's location
    glm::vec3 elementPos = element->Position();
    glm::vec3 elementSize = element->Size();

    element->ResetModelMatrix();
    element->SetPosition(Position() - Size() + elementPos);
    element->SetSize(elementSize);
    element->SetTranslationBounds(translationBounds_.x, translationBounds_.y, translationBounds_.z, translationBounds_.w);

    if (std::dynamic_pointer_cast<ZUIText>(element)) element->SetShader(zenith::UI()->TextShader());
    else element->SetShader(zenith::UI()->UIShader());

    element->SetParent(this);

    children_.push_back(element);
}

bool ZUIElement::RemoveChild(std::shared_ptr<ZUIElement> element)
{
    bool success = false;
    for (auto it = children_.begin(); it != children_.end(); it++)
    {
        if ((*it) == element)
        {
            (*it)->RemoveParent();
            children_.erase(it); success = true; break;
        }
    }
    return success;
}

void ZUIElement::RemoveParent()
{
    parent_ = nullptr;
}

void ZUIElement::SetSize(glm::vec2 size)
{
    modelMatrix_[0][0] = modelMatrix_[1][1] = modelMatrix_[2][2] = 1.f;
    modelMatrix_ = glm::scale(modelMatrix_, glm::vec3(size, 0.f));
}

void ZUIElement::SetPosition(glm::vec2 position)
{
    modelMatrix_[3] = glm::vec4(0.f, 0.f, 0.f, 1.f);
    modelMatrix_ = glm::translate(modelMatrix_, glm::vec3(position, 0.f));
    ClampToBounds();
}

void ZUIElement::SetRotation(float angle)
{
    modelMatrix_[0][1] = modelMatrix_[0][2] = modelMatrix_[1][0] = modelMatrix_[1][2] =
        modelMatrix_[2][0] = modelMatrix_[2][1] = 0.f;
    modelMatrix_ = glm::rotate(modelMatrix_, angle, glm::vec3(0.f, 0.f, 1.f));
}

void ZUIElement::SetTranslationBounds(float left, float right, float bottom, float top)
{
    translationBounds_ = glm::vec4(left, right, bottom, top);
    for (std::shared_ptr<ZUIElement> child : children_)
    {
        child->SetTranslationBounds(left, right, bottom, top);
    }
}

glm::vec3 ZUIElement::Position()
{
    return glm::vec3(modelMatrix_[3].x,
        modelMatrix_[3].y,
        0.f);
}

glm::vec3 ZUIElement::Size()
{
    glm::mat3 scaleMatrix(modelMatrix_);
    return glm::vec3(glm::length(scaleMatrix[0]),
        glm::length(scaleMatrix[1]),
        glm::length(scaleMatrix[2]));
}

float ZUIElement::Angle()
{
    glm::vec3 rotationAxis; float angle;
    glm::axisAngle(modelMatrix_, rotationAxis, angle);
    return angle;
}

void ZUIElement::Translate(glm::vec2 translation)
{
    modelMatrix_ = glm::translate(modelMatrix_,
        glm::vec3(translation.x / Size().x,
            translation.y / Size().y,
            0.f));
    ClampToBounds();

    // Recursively update child positions
    for (std::shared_ptr<ZUIElement> child : children_)
    {
        child->Translate(translation);
    }
}

void ZUIElement::Rotate(float angle)
{
    modelMatrix_ = glm::rotate(modelMatrix_, angle, glm::vec3(0.f, 0.f, 1.f));

    for (std::shared_ptr<ZUIElement> child : children_)
    {
        child->Rotate(angle);
    }
}

void ZUIElement::Scale(glm::vec2 factor)
{
    modelMatrix_ = glm::scale(modelMatrix_, glm::vec3(factor.x,
        factor.y,
        0.f));
}

bool ZUIElement::TrySelect(glm::vec3 position)
{
    bool selected = false;
    if (enabled_ && Contains(position))
    {
        bool selectedChild = false;
        for (std::shared_ptr<ZUIElement>& child : children_)
        {
            selectedChild = child->TrySelect(position);
        }

        if (!selectedChild)
        {
            std::shared_ptr<ZObjectSelectedEvent> objectSelectEvent(new ZObjectSelectedEvent(id_, position));
            zenith::EventAgent()->QueueEvent(objectSelectEvent);
        }

        selected = true;
    }
    return selected;
}

bool ZUIElement::Contains(glm::vec3 point)
{
    return point.x >= Position().x - Size().x && point.x <= Position().x + Size().x &&
        point.y >= Position().y - Size().y && point.y <= Position().y + Size().y;
}

std::shared_ptr<ZMesh2D> ZUIElement::ElementShape()
{
    static std::shared_ptr<ZMesh2D> mesh = ZMesh2D::NewQuad();
    return mesh;
};

void ZUIElement::CleanUp()
{
    for (std::shared_ptr<ZUIElement> child : children_)
        child->CleanUp();
    children_.clear();
}

void ZUIElement::ClampToBounds()
{
    modelMatrix_[3] = glm::vec4(glm::clamp(modelMatrix_[3][0], translationBounds_.x, translationBounds_.y),
        glm::clamp(modelMatrix_[3][1], translationBounds_.z, translationBounds_.w),
        modelMatrix_[3][2],
        modelMatrix_[3][3]);
}
