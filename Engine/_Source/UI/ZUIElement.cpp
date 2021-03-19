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

#include "ZServices.hpp"
#include "ZScene.hpp"
#include "ZMesh.hpp"
#include "ZAssetStore.hpp"
#include "ZDomain.hpp"
#include "ZUIButton.hpp"
#include "ZUIImage.hpp"
#include "ZUIPanel.hpp"
#include "ZUIText.hpp"
#include "ZUICheckbox.hpp"
#include "ZUIListPanel.hpp"
#include "ZShader.hpp"
#include "ZUIText.hpp"
#include "ZUILayout.hpp"
#include "ZMesh.hpp"
#include "ZRenderTask.hpp"
#include "ZRenderPass.hpp"
#include "ZUniformBuffer.hpp"
#include "ZWindowResizeEvent.hpp"
#include "ZObjectSelectedEvent.hpp"
#include "ZWindowResizeEvent.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_interpolation.hpp>

ZIDSequence ZUIElement::idGenerator_("ZUI");

ZUIElement::ZUIElement(const glm::vec2& position, const glm::vec2& scale) : modelMatrix_(1.f), projectionMatrix_(1.f)
{
    id_ = "ZUI_" + std::to_string(idGenerator_.Next());
    type_ = ZUIElementType::Unknown;
    SetPosition(position); SetSize(scale);
}

ZUIElement::ZUIElement(const ZUIElementOptions& options) : modelMatrix_(1.f), projectionMatrix_(1.f) {
    id_ = "ZUI_" + std::to_string(idGenerator_.Next());
    type_ = ZUIElementType::Unknown;
    options_ = options;
}

ZUIElement::~ZUIElement()
{
    CleanUp();
}

void ZUIElement::Initialize() {
    auto scene = Scene();
    if (!scene)
    {
        LOG("ZUIElement was not attached to scene during initialization", ZSeverity::Error);
        return;
    }

    uniformBuffer_ = ZUniformBuffer::Create(ZUniformBufferType::UI, sizeof(ZUIUniforms));

    if (!options_.texture) {
        SetTexture(ZTexture::Default());
    }

    SetRect(options_.rect);
    RecalculateProjectionMatrix();

    auto mesh = ElementShape();
    auto isInstanced = mesh->Instanced();

    uniformBuffer_->Update(offsetof(ZUIUniforms, color), sizeof(options_.color), glm::value_ptr(options_.color));
    uniformBuffer_->Update(offsetof(ZUIUniforms, borderColor), sizeof(options_.border.color), glm::value_ptr(options_.border.color));
    uniformBuffer_->Update(offsetof(ZUIUniforms, borderWidth), sizeof(options_.border.width), &options_.border.width);
    uniformBuffer_->Update(offsetof(ZUIUniforms, borderRadius), sizeof(options_.border.radius), &options_.border.radius);
    uniformBuffer_->Update(offsetof(ZUIUniforms, instanced), sizeof(isInstanced), &isInstanced);


    ZRenderStateGroupWriter writer;
    writer.Begin();
    writer.SetBlending(ZBlendMode::Transluscent);
    writer.SetFullScreenLayer(ZFullScreenLayer::UI);
    writer.SetRenderLayer(ZRenderLayer::UI);
    writer.BindTexture(options_.texture);
    writer.BindUniformBuffer(uniformBuffer_);
    renderState_ = writer.End();

    ZServices::EventAgent()->Subscribe(this, &ZUIElement::OnWindowResized);
}

void ZUIElement::Initialize(const std::shared_ptr<ZOFNode>& root)
{
    auto node = std::static_pointer_cast<ZOFObjectNode>(root);
    if (!node)
    {
        LOG("Could not initalize ZUIElement", ZSeverity::Error);
        return;
    }
    auto scene = Scene();
    if (!scene)
    {
        LOG("ZUIElement was not attached to scene during initialization", ZSeverity::Error);
        return;
    }

    id_ = node->id;

    ZOFPropertyMap props = node->properties;

    if (props.find("positioning") != props.end() && props["positioning"]->HasValues())
    {
        std::shared_ptr<ZOFString> positioningProp = props["positioning"]->Value<ZOFString>(0);
        options_.positioning = positioningProp->value == "Relative" ? ZPositioning::Relative : ZPositioning::Absolute;
    }

    if (props.find("scaling") != props.end() && props["scaling"]->HasValues())
    {
        std::shared_ptr<ZOFString> scalingProp = props["scaling"]->Value<ZOFString>(0);
        options_.scaling = scalingProp->value == "Relative" ? ZPositioning::Relative : ZPositioning::Absolute;
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
        if (ZServices::AssetStore()->HasTexture(texProp->value))
            options_.texture = ZServices::AssetStore()->GetTexture(texProp->value);
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

void ZUIElement::Prepare(double deltaTime, unsigned int zOrder)
{
    // Only render the top level elements that are not hidden. The children will
    // be rendered within the respective parent elements.
    if (options_.hidden) return;

    SetZOrder(zOrder);

    std::shared_ptr<ZMesh2D> mesh = ElementShape();

    ZPR_SESSION_COLLECT_VERTICES(mesh->Vertices().size());

    auto meshState = mesh->RenderState();

    ZDrawCall drawCall = ZDrawCall::Create(ZMeshDrawStyle::TriangleFan);
    auto uiTask = ZRenderTask::Compile(drawCall,
        { meshState, renderState_ },
        ZRenderPass::UI()
    );
    uiTask->Submit({ ZRenderPass::UI() });
}

unsigned int ZUIElement::PrepareChildren(double deltaTime, unsigned int zOrder)
{
    if (options_.hidden) return zOrder;

    unsigned int lastZOrder = zOrder;
    for (auto it = children_.begin(); it != children_.end(); it++)
    {
        it->second->Prepare(deltaTime, ++lastZOrder);
        lastZOrder = it->second->PrepareChildren(deltaTime, lastZOrder);
    }
    return lastZOrder;
}

void ZUIElement::AddChild(const std::shared_ptr<ZUIElement>& element)
{ 
    children_[element->ID()] = element;

    element->SetOpacity(Opacity(), true);
    element->SetTranslationBounds(options_.translationBounds.x, options_.translationBounds.y, options_.translationBounds.z, options_.translationBounds.w);

    element->SetParent(shared_from_this());
    element->SetScene(Scene());

    LayoutChild(element);
}

void ZUIElement::RemoveChild(const std::shared_ptr<ZUIElement>& element, bool recurse)
{
    if (children_.find(element->ID()) != children_.end())
    {
        element->RemoveParent();
        children_.erase(element->ID());
        if (options_.layout) {
            options_.layout->RemoveRect(element->ID());
        }
    }

    if (recurse) {
        for (auto it = children_.begin(), end = children_.end(); it != end; it++)
        {
            it->second->RemoveChild(element, recurse);
        }
    }
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
    parent_.reset();
}

std::shared_ptr<ZScene> ZUIElement::Scene() const
{
    return scene_.lock();
}

ZRect ZUIElement::PaddedRect() const
{
    auto rect = options_.calculatedRect;
    rect.size -= options_.padding * 2.f;
    rect.position += options_.padding;
    return rect;
}

std::shared_ptr<ZUIElement> ZUIElement::Parent() const
{
    return parent_.lock();
}

void ZUIElement::SetPadding(const glm::vec2& padding)
{
    options_.padding = padding;
    if (options_.layout) {
        options_.layout->SetDimensions(PaddedRect());
    }
}

void ZUIElement::SetRect(const ZRect& rect, const ZRect& relativeTo)
{
    SetSize(rect.size, relativeTo);
    SetPosition(rect.position, relativeTo);
    if (options_.layout) {
        options_.layout->SetDimensions(PaddedRect());
    }
    OnRectChanged();
}

void ZUIElement::SetSize(const glm::vec2& size, const ZRect& relativeTo)
{
    auto scene = Scene();
    if (!scene) return;

    if (options_.scaling == ZPositioning::Relative) {
        options_.rect.size = size;
        options_.calculatedRect.size = ZUIHelpers::RelativeToAbsoluteCoords(
            size,
            relativeTo.size == glm::vec2(0.f) ? scene->Domain()->Resolution() : relativeTo.size
        );
    }
    else {
        options_.rect.size = size;
        options_.calculatedRect.size = options_.rect.size;
    }

    uniformBuffer_->Update(offsetof(ZUIUniforms, pixelSize), sizeof(options_.calculatedRect.size), glm::value_ptr(options_.calculatedRect.size));

    ClampToSizeLimits();
    RecalculateModelMatrix();
}

void ZUIElement::SetPosition(const glm::vec2& position, const ZRect& relativeTo)
{
    auto scene = Scene();
    if (!scene) return;

    if (options_.positioning == ZPositioning::Relative) {
        auto parent = Parent();
        glm::vec2 offset = parent && !parent->options_.layout ? parent->options_.calculatedRect.position : glm::vec2(0.f);
        options_.rect.position = position;
        options_.calculatedRect.position = offset + ZUIHelpers::RelativeToAbsoluteCoords(
            position,
            relativeTo.size == glm::vec2(0.f) ? scene->Domain()->Resolution() : relativeTo.size
        );
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

void ZUIElement::SetTexture(const ZTexture::ptr& texture)
{
    options_.texture = texture;

    ZRenderStateGroupWriter writer(renderState_);
    writer.Begin();
    writer.ClearTextures();
    writer.BindTexture(options_.texture);
    renderState_ = writer.End();
}

void ZUIElement::SetBorder(const ZUIBorder& border)
{
    options_.border = border;
    uniformBuffer_->Update(offsetof(ZUIUniforms, borderColor), sizeof(options_.border.color), glm::value_ptr(options_.border.color));
    uniformBuffer_->Update(offsetof(ZUIUniforms, borderWidth), sizeof(options_.border.width), &options_.border.width);
    uniformBuffer_->Update(offsetof(ZUIUniforms, borderRadius), sizeof(options_.border.radius), &options_.border.radius);
}

void ZUIElement::SetTranslationBounds(float left, float right, float bottom, float top)
{
    options_.translationBounds = glm::vec4(left, right, bottom, top);
    for (auto it = children_.begin(); it != children_.end(); it++)
    {
        it->second->SetTranslationBounds(left, right, bottom, top);
    }
}

void ZUIElement::SetColor(const glm::vec4& newColor)
{
    options_.color = newColor;
    uniformBuffer_->Update(offsetof(ZUIUniforms, color), sizeof(options_.color), glm::value_ptr(options_.color));
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

void ZUIElement::SetFlipped(bool flipped)
{
    options_.flipped = flipped;
    RecalculateProjectionMatrix();
}

void ZUIElement::SetZOrder(unsigned int zOrder)
{
    ZRenderStateGroupWriter writer(renderState_);
    writer.Begin();
    writer.SetRenderDepth(zOrder);
    renderState_ = writer.End();
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
    options_.rect.size *= factor;
    options_.calculatedRect.size *= factor;
    ClampToSizeLimits();
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
            ZServices::EventAgent()->Queue(objectSelectEvent);
        }

        selected = true;
    }
    return selected;
}

bool ZUIElement::Contains(const glm::vec2& point)
{
    return options_.calculatedRect.Contains(point);
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
        RemoveChild(it->second);
    }
    ZServices::EventAgent()->Unsubscribe(this, &ZUIElement::OnWindowResized);
}

void ZUIElement::ClampToSizeLimits()
{
    if (options_.minSize.x > 0) {
        options_.calculatedRect.size.x =
            glm::clamp(options_.calculatedRect.size.x, options_.minSize.x, std::numeric_limits<float>::max());
    }
    if (options_.minSize.y > 0) {
        options_.calculatedRect.size.y =
            glm::clamp(options_.calculatedRect.size.y, options_.minSize.y, std::numeric_limits<float>::max());
    }
    if (options_.maxSize.x > 0) {
        options_.calculatedRect.size.x =
            glm::clamp(options_.calculatedRect.size.x, 0.f, options_.maxSize.x);
    }
    if (options_.maxSize.y > 0) {
        options_.calculatedRect.size.y =
            glm::clamp(options_.calculatedRect.size.y, 0.f, options_.maxSize.y);
    }
}

void ZUIElement::ClampToBounds()
{
    auto scene = Scene();
    if (!scene) return;

    glm::vec2 resolution = scene->Domain()->Resolution();

    options_.calculatedRect.position.x = 
        glm::sign(options_.calculatedRect.position.x) *
        glm::clamp(glm::abs(options_.calculatedRect.position.x),
            options_.translationBounds.x * resolution.x,
            options_.translationBounds.y * resolution.x);
    options_.calculatedRect.position.y =
        glm::sign(options_.calculatedRect.position.y) * 
        glm::clamp(glm::abs(options_.calculatedRect.position.y),
            options_.translationBounds.z * resolution.y,
            options_.translationBounds.w * resolution.y);
}

void ZUIElement::RecalculateModelMatrix()
{
    // By default we want to draw with the top left as (0, 0), so we use an identity vector to flip since openGL coordinates
    // begin at the bottom left and by default textures must be flipped to orient correctly. This might
    // do better as a global vector that changes based on the graphics implementation
    glm::vec3 flipVector = options_.flipped ? glm::vec3(1.f) : glm::vec3(1.f, -1.f, 1.f);
    glm::mat4 scale = glm::scale(glm::mat4(1.f), glm::vec3(options_.calculatedRect.size * 0.5f, 0.f) * flipVector);
    glm::mat4 rotate = glm::rotate(glm::mat4(1.f), options_.orientation, glm::vec3(0.f, 0.f, 1.f));
    glm::mat4 translate = glm::translate(glm::mat4(1.f), glm::vec3(options_.calculatedRect.position + options_.calculatedRect.size * 0.5f, 0.f));
    modelMatrix_ = translate * rotate * scale;

    uniformBuffer_->Update(offsetof(ZUIUniforms, M), sizeof(modelMatrix_), glm::value_ptr(modelMatrix_));
}

void ZUIElement::RecalculateProjectionMatrix()
{
    if (auto scene = Scene()) {
        glm::vec2 resolution = scene->Domain()->Resolution();
        projectionMatrix_ = glm::ortho(0.f, (float)resolution.x, (float)resolution.y, 0.f, -1.f, 1.f);
        uniformBuffer_->Update(offsetof(ZUIUniforms, P), sizeof(projectionMatrix_), glm::value_ptr(projectionMatrix_));
    }
}

void ZUIElement::LayoutChild(const std::shared_ptr<ZUIElement>& element, bool force)
{
    auto scene = Scene();
    if (!scene) return;

    auto paddedRect = PaddedRect();
    element->SetRect(element->Rect(), paddedRect);
    if (options_.layout) {
        ZRect rect = options_.layout->GetRect(element->ID(), element->CalculatedRect().size, force);
        if (element->Positioning() == ZPositioning::Relative) {
            rect.position = ZUIHelpers::AbsoluteToRelativeCoords(
                rect.position,
                options_.calculatedRect.size == glm::vec2(0.f) ? scene->Domain()->Resolution() : paddedRect.size
            );
        }
        if (element->Scaling() == ZPositioning::Relative) {
            rect.size = ZUIHelpers::AbsoluteToRelativeCoords(
                rect.size,
                options_.calculatedRect.size == glm::vec2(0.f) ? scene->Domain()->Resolution() : paddedRect.size
            );
            // We want to lock the relative position of the rect if it is already at its minimum size
            if (element->options_.calculatedRect.size.x <= element->options_.minSize.x
                || element->options_.calculatedRect.size.y <= element->options_.minSize.y) {
                rect.size = element->options_.rect.size;
            }
        }
        element->SetRect(rect, paddedRect);
    }

    LayoutChildren(element, true);
}

void ZUIElement::LayoutChildren(const std::shared_ptr<ZUIElement>& element, bool force)
{
    // If there's a layout for the given element we want to traverse the children in order of the calculated rects
    // in the layout object, otherwise we might update the rect for an element and miss updating dependent rects
    // in the layout if those objects were already traversed.
    if (element->options_.layout) {
        std::vector<std::shared_ptr<ZUIElement>> layoutList;
        for (auto it = element->options_.layout->Rects().begin(); it != element->options_.layout->Rects().end(); it++) {
            auto elIt = element->children_.find((*it).id);
            if (elIt != element->children_.end()) {
                layoutList.push_back(elIt->second);
            }
        }
        for (auto it = layoutList.begin(); it != layoutList.end(); it++) {
            element->LayoutChild((*it), force);
        }
    }
    else {
        for (auto it = element->children_.begin(); it != element->children_.end(); it++) {
            element->LayoutChild(it->second, force);
        }
    }
}

void ZUIElement::OnWindowResized(const std::shared_ptr<ZWindowResizeEvent>& event)
{
    RecalculateProjectionMatrix();

    auto parent = Parent();
    if (!parent || !parent->options_.layout) {
        SetRect(options_.rect, parent ? parent->PaddedRect() : ZRect());
    }

    for (auto it = children_.begin(); it != children_.end(); it++) {
        LayoutChild(it->second, true);
    }
}

ZUIElementList ZUIElement::Load(std::shared_ptr<ZOFTree> data, const std::shared_ptr<ZScene>& scene)
{
    ZUIElementList uiElements;
    for (ZOFChildMap::iterator it = data->children.begin(); it != data->children.end(); it++)
    {
        std::shared_ptr<ZOFNode> node = it->second;
        if (zenith::strings::HasUIPrefix(node->id))
        {
            std::shared_ptr<ZOFObjectNode> uiNode = std::static_pointer_cast<ZOFObjectNode>(node);
            ZOFPropertyMap props = uiNode->properties;

            std::shared_ptr<ZUIElement> element;

            if (props.find("type") != props.end() && props["type"]->HasValues())
            {
                std::shared_ptr<ZOFString> typeProp = props["type"]->Value<ZOFString>(0);
                element = ZUIElement::Create(typeProp->value);
                element->SetScene(scene);
                element->Initialize(uiNode);
            }

            // Recursively create children if there are any nested UI nodes
            if (element)
            {
                ZUIElementList uiChildren = Load(uiNode, scene);
                for (ZUIElementList::iterator it = uiChildren.begin(); it != uiChildren.end(); it++)
                {
                    element->AddChild(*it);
                }

                uiElements.push_back(element);
            }
        }
    }
    return uiElements;
}

std::shared_ptr<ZUIElement> ZUIElement::Create(const std::string& type)
{
    if (type == "Button") {
        return ZUIButton::Create();
    }
    else if (type == "Image") {
        return ZUIImage::Create();
    }
    else if (type == "Panel") {
        return ZUIPanel::Create();
    }
    else if (type == "Text") {
        return ZUIText::Create();
    }
    else if (type == "Checkbox") {
        return ZUICheckBox::Create();
    }
    else if (type == "ListPanel") {
        return ZUIListPanel::Create();
    }
    LOG("Could not create a UI element of type " + type, ZSeverity::Error);
    return nullptr;
}
