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
#include "ZUILayout.hpp"
#include "ZEventAgent.hpp"
#include "ZObjectSelectedEvent.hpp"
#include "ZWindowResizeEvent.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_interpolation.hpp>

ZUIElement::ZUIElement(const glm::vec2& position, const glm::vec2& scale) : modelMatrix_(1.f)
{
    id_ = "ZUI_" + zenith::IDSequence()->Next();
    type_ = ZUIElementType::Unknown;
    SetPosition(position); SetSize(scale);
}

ZUIElement::ZUIElement(const ZUIElementOptions& options) : modelMatrix_(1.f) {
    id_ = "ZUI_" + zenith::IDSequence()->Next();
    type_ = ZUIElementType::Unknown;
    options_ = options;
}

void ZUIElement::Initialize() {
    if (options_.shader == nullptr) {
        options_.shader = zenith::UI()->UIShader();
    }
    if (options_.texture.id == 0) {
        options_.texture = zenith::Graphics()->Strategy()->LoadDefaultTexture();
    }

    SetRect(options_.rect);

    ZEventDelegate windowResizeDelegate = fastdelegate::MakeDelegate(this, &ZUIElement::OnWindowResized);
    zenith::EventAgent()->AddListener(windowResizeDelegate, ZWindowResizeEvent::Type);
}

void ZUIElement::Initialize(const std::shared_ptr<ZOFNode>& root)
{
    std::shared_ptr<ZOFObjectNode> node = std::static_pointer_cast<ZOFObjectNode>(root);
    if (node == nullptr)
    {
        zenith::Log("Could not initalize ZUIElement", ZSeverity::Error);
        return;
    }

    id_ = node->id;

    ZOFPropertyMap props = node->properties;

    if (props.find("positioning") != props.end() && props["positioning"]->HasValues())
    {
        std::shared_ptr<ZOFString> positioningProp = props["positioning"]->Value<ZOFString>(0);
        options_.positioning = positioningProp->value == "Relative" ? ZPositioning::Relative : ZPositioning::Absolute;
    }

    if (props.find("scale") != props.end() && props["scale"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> scaleProp = props["scale"]->Value<ZOFNumberList>(0);
        float x = scaleProp->value[0] * 0.01;
        float y = scaleProp->value[1] * 0.01;
        options_.rect.size = glm::vec2(x, y);
    }

    if (props.find("position") != props.end() && props["position"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> posProp = props["position"]->Value<ZOFNumberList>(0);
        float x = posProp->value[0] * 0.01;
        float y = posProp->value[1] * 0.01;
        options_.rect.position = glm::vec2(x, y);
    }

    if (props.find("anchor") != props.end() && props["anchor"]->HasValues())
    {
        std::shared_ptr<ZOFString> hAnchorProp = props["anchor"]->Value<ZOFString>(0);
        std::shared_ptr<ZOFString> vAnchorProp = props["anchor"]->Value<ZOFString>(1);
        // TODO: A parent pointer is needed to determine the bounds and therefore the anchor relative positioning.
        // if there is no parent we simply anchor to the screen/resolution bounds
    }

    if (props.find("color") != props.end() && props["color"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> colorProp = props["color"]->Value<ZOFNumberList>(0);
        options_.color = glm::vec4(colorProp->value[0], colorProp->value[1], colorProp->value[2], colorProp->value[3]);
    }

    if (props.find("isHidden") != props.end() && props["isHidden"]->HasValues())
    {
        std::shared_ptr<ZOFString> hiddenProp = props["isHidden"]->Value<ZOFString>(0);
        options_.hidden = hiddenProp->value == "Yes";
    }

    if (props.find("isEnabled") != props.end() && props["isEnabled"]->HasValues())
    {
        std::shared_ptr<ZOFString> enabledProp = props["isEnabled"]->Value<ZOFString>(0);
        options_.enabled = enabledProp->value == "Yes";
    }

    if (props.find("texture") != props.end() && props["texture"]->HasValues())
    {
        std::shared_ptr<ZOFString> texProp = props["texture"]->Value<ZOFString>(0);
        if (zenith::Graphics()->Textures().find(texProp->value) != zenith::Graphics()->Textures().end())
            options_.texture = zenith::Graphics()->Textures()[texProp->value];
    }

    if (props.find("borderWidth") != props.end() && props["borderWidth"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> borderWidthProp = props["borderWidth"]->Value<ZOFNumber>(0);
        options_.border.width = borderWidthProp->value;
    }

    if (props.find("borderColor") != props.end() && props["borderColor"]->HasValues())
    {
        std::shared_ptr<ZOFNumberList> borderColorProp = props["borderColor"]->Value<ZOFNumberList>(0);
        options_.border.color = glm::vec4(borderColorProp->value[0], borderColorProp->value[1], borderColorProp->value[2], borderColorProp->value[3]);
    }

    if (props.find("borderRadius") != props.end() && props["borderRadius"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> borderRadiusProp = props["borderRadius"]->Value<ZOFNumber>(0);
        options_.border.radius = borderRadiusProp->value;
    }

    if (props.find("opacity") != props.end() && props["opacity"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> opacityProp = props["opacity"]->Value<ZOFNumber>(0);
        options_.opacity = opacityProp->value;
        options_.color.a = options_.opacity;
        options_.border.color.a = options_.opacity;
    }

    if (props.find("zIndex") != props.end() && props["zIndex"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> zIndexProp = props["zIndex"]->Value<ZOFNumber>(0);
        id_ = id_.substr(0, id_.find("_")) + "_" + std::to_string(static_cast<int>(zIndexProp->value)) + id_.substr(id_.find("_"));
    }

    Initialize();
}

void ZUIElement::AddChild(const std::shared_ptr<ZUIElement>& element)
{ 
    element->SetOpacity(Opacity(), true);
    element->SetTranslationBounds(options_.translationBounds.x, options_.translationBounds.y, options_.translationBounds.z, options_.translationBounds.w);

    element->SetParent(this);

    LayoutChild(element);

    children_[element->ID()] = element;
}

bool ZUIElement::RemoveChild(const std::shared_ptr<ZUIElement>& element)
{
    bool success = false;
    if (children_.find(element->ID()) != children_.end())
    {
        element->RemoveParent();
        children_.erase(element->ID());
        if (options_.layout) {
            options_.layout->RemoveRect(element->ID());
        }
        success = true;
    }
    return success;
}

void ZUIElement::DoRecursiveChildUpdate(std::function<void(std::shared_ptr<ZUIElement>)> callback)
{
    for (auto it = children_.begin(); it != children_.end(); it++)
    {
        callback(it->second);
        if (it->second->HasChildren())
        {
            it->second->DoRecursiveChildUpdate(callback);
        }
    }
}

void ZUIElement::RemoveParent()
{
    parent_ = nullptr;
}

void ZUIElement::SetRect(const ZRect& rect, const ZRect& relativeTo)
{
    SetSize(rect.size, relativeTo);
    SetPosition(rect.position, relativeTo);
    if (options_.layout) {
        options_.layout->SetDimensions(options_.calculatedRect);
    }
}

void ZUIElement::SetSize(const glm::vec2& size, const const ZRect& relativeTo)
{
    if (options_.positioning == ZPositioning::Relative) {
        options_.rect.size = size;
        options_.calculatedRect.size = ZUIHelpers::RelativeToAbsoluteCoords(size, relativeTo.size);
    }
    else {
        options_.rect.size = size;
        options_.calculatedRect.size = options_.rect.size;
    }
    RecalculateModelMatrix();
}

void ZUIElement::SetPosition(const glm::vec2& position, const const ZRect& relativeTo)
{
    if (options_.positioning == ZPositioning::Relative) {
        options_.rect.position = position;
        options_.calculatedRect.position = ZUIHelpers::RelativeToAbsoluteCoords(position, relativeTo.size);
    }
    else {
        options_.rect.position = position;
        options_.calculatedRect.position = options_.rect.position;
    }
    ClampToBounds();
    RecalculateModelMatrix();
}

void ZUIElement::SetRotation(float angle)
{
    options_.orientation = angle;
    RecalculateModelMatrix();
}

void ZUIElement::SetTranslationBounds(float left, float right, float bottom, float top)
{
    options_.translationBounds = glm::vec4(left, right, bottom, top);
    for (auto it = children_.begin(); it != children_.end(); it++)
    {
        it->second->SetTranslationBounds(left, right, bottom, top);
    }
}

void ZUIElement::SetOpacity(float opacity, bool relativeToAlpha)
{
    options_.opacity = opacity;
    options_.color.a = relativeToAlpha ? options_.color.a * options_.opacity : options_.opacity;
    options_.border.color.a = relativeToAlpha ? options_.border.color.a * options_.opacity : options_.opacity;
    for (auto it = children_.begin(); it != children_.end(); it++)
    {
        it->second->SetOpacity(opacity, relativeToAlpha);
    }
}

void ZUIElement::Translate(const glm::vec2& translation)
{
    options_.calculatedRect.position += translation;
    ClampToBounds();
    RecalculateModelMatrix();

    for (auto it = children_.begin(); it != children_.end(); it++)
    {
        it->second->Translate(translation);
    }
}

void ZUIElement::Rotate(float angle)
{
    options_.orientation += angle;
    RecalculateModelMatrix();

    for (auto it = children_.begin(); it != children_.end(); it++)
    {
        it->second->Rotate(angle);
    }
}

void ZUIElement::Scale(const glm::vec2& factor)
{
    options_.calculatedRect.size *= factor;
    RecalculateModelMatrix();
}

bool ZUIElement::TrySelect(const glm::vec3& position)
{
    bool selected = false;
    if (options_.enabled && Contains(position))
    {
        bool selectedChild = false;
        for (auto it = children_.begin(); it != children_.end(); it++)
        {
            selectedChild = it->second->TrySelect(position);
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

bool ZUIElement::Contains(const glm::vec3& point)
{
    return point.x >= options_.calculatedRect.position.x - options_.calculatedRect.size.x &&
        point.x <= options_.calculatedRect.position.x + options_.calculatedRect.size.x &&
        point.y >= options_.calculatedRect.position.y - options_.calculatedRect.size.y &&
        point.y <= options_.calculatedRect.position.y + options_.calculatedRect.size.y;
}

std::shared_ptr<ZMesh2D> ZUIElement::ElementShape()
{
    static std::shared_ptr<ZMesh2D> mesh = ZMesh2D::NewQuad();
    return mesh;
};

void ZUIElement::CleanUp()
{
    ZUIElementMap childrenCopy(children_);
    for (auto it = childrenCopy.begin(); it != childrenCopy.end(); it++)
    {
        it->second->CleanUp();
        RemoveChild(it->second);
    }
}

void ZUIElement::ClampToBounds()
{
    options_.calculatedRect.position.x = 
        glm::sign(options_.calculatedRect.position.x) *
        glm::clamp(glm::abs(options_.calculatedRect.position.x),
            options_.translationBounds.x * zenith::Domain()->ResolutionX(),
            options_.translationBounds.y * zenith::Domain()->ResolutionX());
    options_.calculatedRect.position.y =
        glm::sign(options_.calculatedRect.position.y) * 
        glm::clamp(glm::abs(options_.calculatedRect.position.y),
            options_.translationBounds.z * zenith::Domain()->ResolutionY(),
            options_.translationBounds.w * zenith::Domain()->ResolutionY());
}

void ZUIElement::RecalculateModelMatrix()
{
    glm::mat4 scale = glm::scale(glm::mat4(1.f), glm::vec3(options_.calculatedRect.size * 0.5f, 0.f));
    glm::mat4 rotate = glm::rotate(glm::mat4(1.f), options_.orientation, glm::vec3(0.f, 0.f, 1.f));
    glm::mat4 translate = glm::translate(glm::mat4(1.f), glm::vec3(options_.calculatedRect.position + options_.calculatedRect.size * 0.5f, 0.f));
    modelMatrix_ = translate * rotate * scale;
}

void ZUIElement::LayoutChild(const std::shared_ptr<ZUIElement>& element, bool force)
{
    element->SetRect(element->Rect(), options_.calculatedRect);
    if (options_.layout) {
        ZRect rect = options_.layout->GetRect(element->ID(), element->CalculatedRect().size, force);
        if (element->Positioning() == ZPositioning::Relative) {
            rect = ZUIHelpers::AbsoluteToRelativeRect(rect, options_.calculatedRect);
        }
        element->SetRect(rect, options_.calculatedRect);
    }
}

void ZUIElement::OnWindowResized(const std::shared_ptr<ZEvent>& event)
{
    if (!parent_ || !parent_->options_.layout) {
        SetRect(options_.rect, parent_ ? parent_->options_.calculatedRect : ZRect());
    }
    if (options_.layout) {
        for (auto it = children_.begin(); it != children_.end(); it++) {
            LayoutChild(it->second, true);
        }
    }
}
