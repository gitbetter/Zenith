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
#include "ZServices.hpp"
#include "ZAssets.hpp"
#include "ZScene.hpp"
#include "ZMesh.hpp"
#include "ZDomain.hpp"
#include "ZShader.hpp"
#include "ZUILayout.hpp"
#include "ZMesh.hpp"
#include "ZRenderTask.hpp"
#include "ZRenderPass.hpp"
#include "ZUniformBuffer.hpp"
#include "ZWindowResizeEvent.hpp"
#include "ZElementSelectedEvent.hpp"
#include "ZWindowResizeEvent.hpp"

#include "ZUIButton.hpp"
#include "ZUICanvas.hpp"
#include "ZUICheckbox.hpp"
#include "ZUIImage.hpp"
#include "ZUIInputField.hpp"
#include "ZUILabeledElement.hpp"
#include "ZUIListPanel.hpp"
#include "ZUIPanel.hpp"
#include "ZUIText.hpp"
#include "ZUIHelpers.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_interpolation.hpp>

ZUIElementType ElementTypeFromString(const std::string& type)
{
	if (type == "Button")
	{
		return ZUIElementType::Button;
	}
	else if (type == "Canvas")
	{
		return ZUIElementType::Canvas;
	}
	else if (type == "Checkbox")
	{
		return ZUIElementType::CheckBox;
	}
	else if (type == "Image")
	{
		return ZUIElementType::Image;
	}
	else if (type == "InputField")
	{
		return ZUIElementType::InputField;
	}
	else if (type == "LabeledElement")
	{
		return ZUIElementType::LabeledElement;
	}
	else if (type == "ListPanel")
	{
		return ZUIElementType::ListPanel;
	}
	else if (type == "Panel")
	{
		return ZUIElementType::Panel;
	}
	else if (type == "Text")
	{
		return ZUIElementType::Text;
	}
	return ZUIElementType::Unknown;
}

ZIDSequence ZUIElement::idGenerator_;

ZUIElement::ZUIElement()
{
    name = "UIElement_" + std::to_string(idGenerator_.Next());
}

ZHUIElement ZUIElementManager::Deserialize(const ZOFHandle& dataHandle, const std::shared_ptr<ZOFObjectNode>& dataNode, const std::shared_ptr<ZScene>& scene)
{
	if (dataNode->type != ZOFObjectType::UI)
	{
		return ZHUIElement();
	}

	ZHUIElement restoreHandle(dataHandle.value);

    ZOFPropertyMap props = dataNode->properties;

	if (props.find("type") != props.end() && props["type"]->HasValues())
	{
		std::shared_ptr<ZOFString> typeProp = props["type"]->Value<ZOFString>(0);
		restoreHandle = Create(ElementTypeFromString(typeProp->value), restoreHandle);
	}

	ZUIElement* element = resourcePool_.Get(restoreHandle);

	if (element == nullptr)
	{
		return ZHUIElement();
	}

	if (props.find("name") != props.end() && props["name"]->HasValues())
	{
		std::shared_ptr<ZOFString> nameProp = props["name"]->Value<ZOFString>(0);
		element->name = nameProp->value;
	}

    if (props.find("positioning") != props.end() && props["positioning"]->HasValues())
    {
        std::shared_ptr<ZOFString> positioningProp = props["positioning"]->Value<ZOFString>(0);
		element->options.positioning = positioningProp->value == "Relative" ? ZPositioning::Relative : ZPositioning::Absolute;
    }

    if (props.find("scaling") != props.end() && props["scaling"]->HasValues())
    {
        std::shared_ptr<ZOFString> scalingProp = props["scaling"]->Value<ZOFString>(0);
		element->options.scaling = scalingProp->value == "Relative" ? ZPositioning::Relative : ZPositioning::Absolute;
	}

	if (props.find("scale") != props.end() && props["scale"]->HasValues())
	{
		std::shared_ptr<ZOFNumberList> scaleProp = props["scale"]->Value<ZOFNumberList>(0);
		float x = scaleProp->value[0] * 0.01;
		float y = scaleProp->value[1] * 0.01;
		element->options.rect.size = glm::vec2(x, y);
	}

	if (props.find("position") != props.end() && props["position"]->HasValues())
	{
		std::shared_ptr<ZOFNumberList> posProp = props["position"]->Value<ZOFNumberList>(0);
		float x = posProp->value[0] * 0.01;
		float y = posProp->value[1] * 0.01;
		element->options.rect.position = glm::vec2(x, y);
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
		element->options.color = glm::vec4(colorProp->value[0], colorProp->value[1], colorProp->value[2], colorProp->value[3]);
	}

	if (props.find("isHidden") != props.end() && props["isHidden"]->HasValues())
	{
		std::shared_ptr<ZOFString> hiddenProp = props["isHidden"]->Value<ZOFString>(0);
		element->options.hidden = hiddenProp->value == "Yes";
    }

    if (props.find("isEnabled") != props.end() && props["isEnabled"]->HasValues())
    {
        std::shared_ptr<ZOFString> enabledProp = props["isEnabled"]->Value<ZOFString>(0);
		element->options.enabled = enabledProp->value == "Yes";
	}

	if (props.find("texture") != props.end() && props["texture"]->HasValues())
	{
		std::shared_ptr<ZOFNumber> texProp = props["texture"]->Value<ZOFNumber>(0);
		element->options.texture = ZHTexture(texProp->value);
	}

	if (props.find("borderWidth") != props.end() && props["borderWidth"]->HasValues())
	{
		std::shared_ptr<ZOFNumber> borderWidthProp = props["borderWidth"]->Value<ZOFNumber>(0);
		element->options.border.width = borderWidthProp->value;
	}

	if (props.find("borderColor") != props.end() && props["borderColor"]->HasValues())
	{
		std::shared_ptr<ZOFNumberList> borderColorProp = props["borderColor"]->Value<ZOFNumberList>(0);
		element->options.border.color = glm::vec4(borderColorProp->value[0], borderColorProp->value[1], borderColorProp->value[2], borderColorProp->value[3]);
	}

	if (props.find("borderRadius") != props.end() && props["borderRadius"]->HasValues())
	{
		std::shared_ptr<ZOFNumber> borderRadiusProp = props["borderRadius"]->Value<ZOFNumber>(0);
		element->options.border.radius = borderRadiusProp->value;
	}

	if (props.find("opacity") != props.end() && props["opacity"]->HasValues())
	{
		std::shared_ptr<ZOFNumber> opacityProp = props["opacity"]->Value<ZOFNumber>(0);
		element->options.opacity = opacityProp->value;
		element->options.color.a = element->options.opacity;
		element->options.border.color.a = element->options.opacity;
    }

    if (props.find("zIndex") != props.end() && props["zIndex"]->HasValues())
    {
        std::shared_ptr<ZOFNumber> zIndexProp = props["zIndex"]->Value<ZOFNumber>(0);
		element->name = element->name.substr(0, element->name.find("_")) + "_" + std::to_string(static_cast<int>(zIndexProp->value)) + element->name.substr(element->name.find("_"));
    }

	element->OnDeserialize(dataNode);

    Initialize(restoreHandle);

	return restoreHandle;
}

ZHUIElement ZUIElementManager::Create(const ZUIElementType& type, const ZHUIElement& restoreHandle)
{
	ZHUIElement handle(restoreHandle);

	switch (type)
	{
		case ZUIElementType::Button:
		{
			ZUIButton* element = resourcePool_.New<ZUIButton>(handle);
			break;
		}
		case ZUIElementType::Canvas:
		{
			ZUICanvas* element = resourcePool_.New<ZUICanvas>(handle);
			break;
		}
		case ZUIElementType::CheckBox:
		{
			ZUICheckBox* element = resourcePool_.New<ZUICheckBox>(handle);
			break;
		}
		case ZUIElementType::Image:
		{
			ZUIImage* element = resourcePool_.New<ZUIImage>(handle);
			break;
		}
		case ZUIElementType::InputField:
		{
			ZUIInputField* element = resourcePool_.New<ZUIInputField>(handle);
			break;
		}
		case ZUIElementType::LabeledElement:
		{
			ZUILabeledElement* element = resourcePool_.New<ZUILabeledElement>(handle);
			break;
		}
		case ZUIElementType::ListPanel:
		{
			ZUIListPanel* element = resourcePool_.New<ZUIListPanel>(handle);
			break;
		}
		case ZUIElementType::Panel:
		{
			ZUIPanel* element = resourcePool_.New<ZUIPanel>(handle);
			break;
		}
		case ZUIElementType::Text:
		{
			ZUIText* element = resourcePool_.New<ZUIText>(handle);
			break;
		}
		default: break;
	}

    return handle;
}

ZHUIElement ZUIElementManager::Create(const ZUIElementType& type, const ZUIElementOptions& options, const ZHUIElement& restoreHandle, const std::shared_ptr<ZScene>& scene)
{
	ZHUIElement handle(restoreHandle);
	ZUIElement* element = nullptr;

	switch (type)
	{
	case ZUIElementType::Button:
		element = resourcePool_.New<ZUIButton>(handle);
		break;
	case ZUIElementType::Canvas:
		element = resourcePool_.New<ZUICanvas>(handle);
		break;
	case ZUIElementType::CheckBox:
		element = resourcePool_.New<ZUICheckBox>(handle);
		break;
	case ZUIElementType::Image:
		element = resourcePool_.New<ZUIImage>(handle);
		break;
	case ZUIElementType::InputField:
		element = resourcePool_.New<ZUIInputField>(handle);
		break;
	case ZUIElementType::LabeledElement:
		element = resourcePool_.New<ZUILabeledElement>(handle);
		break;
	case ZUIElementType::ListPanel:
		element = resourcePool_.New<ZUIListPanel>(handle);
		break;
	case ZUIElementType::Panel:
		element = resourcePool_.New<ZUIPanel>(handle);
		break;
	case ZUIElementType::Text:
		element = resourcePool_.New<ZUIText>(handle);
		break;
	default: break;
	}

	if (scene != nullptr && element != nullptr) {
		element->options = options;
		SetScene(handle, scene);
		Initialize(handle);
	}

	return handle;
}

void ZUIElementManager::Initialize()
{
	ZServices::EventAgent()->Subscribe(this, &ZUIElementManager::OnWindowResized);
}

void ZUIElementManager::Initialize(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);

	auto scene = Scene(handle);
	if (!scene)
	{
		LOG("ZUIElement was not attached to scene during initialization", ZSeverity::Error);
		return;
	}

	uiElement->uniformBuffer = ZUniformBuffer::Create(ZUniformBufferType::UI, sizeof(ZUIUniforms));

	if (uiElement->options.texture.IsNull())
	{
		SetTexture(handle, ZAssets::TextureManager()->Default());
	}

	SetRect(handle, uiElement->options.rect);
	RecalculateProjectionMatrix(handle);

	auto mesh = ElementShape(handle);
	auto isInstanced = mesh->Instanced();

	uiElement->uniformBuffer->Update(offsetof(ZUIUniforms, color), sizeof(uiElement->options.color), glm::value_ptr(uiElement->options.color));
	uiElement->uniformBuffer->Update(offsetof(ZUIUniforms, borderColor), sizeof(uiElement->options.border.color), glm::value_ptr(uiElement->options.border.color));
	uiElement->uniformBuffer->Update(offsetof(ZUIUniforms, borderWidth), sizeof(uiElement->options.border.width), &uiElement->options.border.width);
	uiElement->uniformBuffer->Update(offsetof(ZUIUniforms, borderRadius), sizeof(uiElement->options.border.radius), &uiElement->options.border.radius);
	uiElement->uniformBuffer->Update(offsetof(ZUIUniforms, instanced), sizeof(isInstanced), &isInstanced);


	ZRenderStateGroupWriter writer;
	writer.Begin();
	writer.SetBlending(ZBlendMode::Transluscent);
	writer.SetFullScreenLayer(ZFullScreenLayer::UI);
	writer.SetRenderLayer(ZRenderLayer::UI);
	writer.BindTexture(uiElement->options.texture);
	writer.BindUniformBuffer(uiElement->uniformBuffer);
	writer.SetShader(uiElement->options.shader);
	uiElement->renderState = writer.End();

	uiElement->OnInitialize();
}

void ZUIElementManager::CleanUp()
{
	ZServices::EventAgent()->Unsubscribe(this, &ZUIElementManager::OnWindowResized);
}

void ZUIElementManager::Update(const ZHUIElement& handle, double deltaTime, unsigned int zOrder)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);

	// Only render the top level elements that are not hidden. The children will
	// be rendered within the respective parent elements.
	if (uiElement->options.hidden) return;

	SetZOrder(handle, zOrder);

	std::shared_ptr<ZMesh2D> mesh = ElementShape(handle);

	ZPR_SESSION_COLLECT_VERTICES(mesh->Vertices().size());

	auto meshState = mesh->renderState;

	ZDrawCall drawCall = ZDrawCall::Create(uiElement->options.drawStyle);
	auto uiTask = ZRenderTask::Compile(drawCall,
		{ meshState, uiElement->renderState },
		ZRenderPass::UI()
	);
	uiTask->Submit({ ZRenderPass::UI() });

	UpdateChildren(handle, deltaTime, zOrder);

	uiElement->OnUpdate(deltaTime, zOrder);
}

unsigned int ZUIElementManager::UpdateChildren(const ZHUIElement& handle, double deltaTime, unsigned int zOrder)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);

	if (uiElement->options.hidden) return zOrder;

	unsigned int lastZOrder = zOrder;
	for (auto it = uiElement->children.begin(); it != uiElement->children.end(); it++)
	{
		Update(it->second, deltaTime, ++lastZOrder);
		lastZOrder = UpdateChildren(it->second, deltaTime, lastZOrder);
	}
	return lastZOrder;
}

std::string ZUIElementManager::Name(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	return uiElement->name;
}

ZUIElementType ZUIElementManager::Type(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	return uiElement->type;
}

bool ZUIElementManager::Enabled(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	return uiElement->options.enabled;
}

bool ZUIElementManager::Hidden(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	return uiElement->options.hidden;
}

bool ZUIElementManager::Selected(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	return uiElement->options.selected;
}

bool ZUIElementManager::Flipped(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	return uiElement->options.flipped;
}

ZPositioning ZUIElementManager::Positioning(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	return uiElement->options.positioning;
}

ZPositioning ZUIElementManager::Scaling(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	return uiElement->options.scaling;
}

glm::vec2 ZUIElementManager::Position(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	return uiElement->options.rect.position;
}

glm::vec2 ZUIElementManager::Size(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	return uiElement->options.rect.size;
}

glm::vec2 ZUIElementManager::MaxSize(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	return uiElement->options.maxSize;
}

glm::vec2 ZUIElementManager::MinSize(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	return uiElement->options.minSize;
}

ZRect ZUIElementManager::Rect(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	return uiElement->options.rect;
}

ZRect ZUIElementManager::CalculatedRect(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	return uiElement->options.calculatedRect;
}

glm::vec2 ZUIElementManager::Padding(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	return uiElement->options.padding;
}

float ZUIElementManager::Angle(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	return uiElement->options.orientation;
}

glm::vec4 ZUIElementManager::Color(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	return uiElement->options.color;
}

float ZUIElementManager::Opacity(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	return uiElement->options.opacity;
}

glm::vec4 ZUIElementManager::TranslationBounds(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	return uiElement->options.translationBounds;
}

const ZHTexture& ZUIElementManager::Texture(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	return uiElement->options.texture;
}

const ZUIBorder& ZUIElementManager::Border(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	return uiElement->options.border;
}

const ZHShader& ZUIElementManager::Shader(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	return uiElement->options.shader;
}

const ZUIElementMap& ZUIElementManager::Children(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	return uiElement->children;
}

const std::shared_ptr<ZUILayout>& ZUIElementManager::Layout(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	return uiElement->options.layout;
}

glm::mat4 ZUIElementManager::ModelMatrix(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	return uiElement->modelMatrix;
}

glm::mat4 ZUIElementManager::ProjectionMatrix(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	return uiElement->projectionMatrix;
}

ZUIElementOptions ZUIElementManager::Options(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null texture handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	return uiElement->options;
}

ZRect ZUIElementManager::PaddedRect(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	auto rect = uiElement->options.calculatedRect;
	rect.size -= uiElement->options.padding * 2.f;
	rect.position += uiElement->options.padding;
	return rect;
}

ZHUIElement ZUIElementManager::Parent(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	return uiElement->parent;;
}

std::shared_ptr<ZScene> ZUIElementManager::Scene(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	return uiElement->scene.lock();
}

void ZUIElementManager::SetPositioning(const ZHUIElement& handle, const ZPositioning& positioning)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	uiElement->options.positioning = positioning;
}

void ZUIElementManager::SetScaling(const ZHUIElement& handle, const ZPositioning& scaling)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	uiElement->options.scaling = scaling;
}

void ZUIElementManager::SetPadding(const ZHUIElement& handle, const glm::vec2& padding)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	uiElement->options.padding = padding;
	if (uiElement->options.layout) {
        uiElement->options.layout->SetDimensions(PaddedRect(handle));
	}
}

void ZUIElementManager::SetRect(const ZHUIElement& handle, const ZRect& rect, const ZRect& relativeTo)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	SetSize(handle, rect.size, relativeTo);
	SetPosition(handle, rect.position, relativeTo);
	if (uiElement->options.layout) {
        uiElement->options.layout->SetDimensions(PaddedRect(handle));
	}
	uiElement->OnRectChanged();
}

void ZUIElementManager::SetSize(const ZHUIElement& handle, const glm::vec2& size, const ZRect& relativeTo)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);

	auto scene = Scene(handle);
	if (!scene) return;

    uiElement->options.rect.size = size;
	if (uiElement->options.scaling == ZPositioning::Relative) {
        uiElement->options.calculatedRect.size = ZUIHelpers::RelativeToAbsoluteCoords(
			size,
			relativeTo.size == glm::vec2(0.f) ? scene->Domain()->Resolution() : relativeTo.size
		);
	}
	else {
        uiElement->options.calculatedRect.size = uiElement->options.rect.size;
	}

	ClampToSizeLimits(handle);

	uiElement->uniformBuffer->Update(offsetof(ZUIUniforms, pixelSize), sizeof(uiElement->options.calculatedRect.size), glm::value_ptr(uiElement->options.calculatedRect.size));

	RecalculateModelMatrix(handle);
}

void ZUIElementManager::SetMaxSize(const ZHUIElement& handle, const glm::vec2& size)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);

    uiElement->options.maxSize = size;
	RecalculateRect(handle, true);
}

void ZUIElementManager::SetPosition(const ZHUIElement& handle, const glm::vec2& position, const ZRect& relativeTo)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);

	auto scene = Scene(handle);
	if (!scene) return;

    uiElement->options.rect.position = position;
	if (uiElement->options.positioning == ZPositioning::Relative) {
		ZHUIElement parentHandle = Parent(handle);
        ZUIElement* parentElement = resourcePool_.Get(parentHandle);
		glm::vec2 offset = parentElement && !parentElement->options.layout ? parentElement->options.calculatedRect.position : glm::vec2(0.f);
        uiElement->options.calculatedRect.position = offset + ZUIHelpers::RelativeToAbsoluteCoords(
			position,
			relativeTo.size == glm::vec2(0.f) ? scene->Domain()->Resolution() : relativeTo.size
		);
	}
	else {
        uiElement->options.calculatedRect.position = uiElement->options.rect.position;
	}
	ClampToBounds(handle);
	RecalculateModelMatrix(handle);
}

void ZUIElementManager::SetRotation(const ZHUIElement& handle, float angle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);

    uiElement->options.orientation = angle;
	RecalculateModelMatrix(handle);
}

void ZUIElementManager::SetTexture(const ZHUIElement& handle, const ZHTexture& texture)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);

    uiElement->options.texture = texture;

	ZRenderStateGroupWriter writer(uiElement->renderState);
	writer.Begin();
	writer.ClearTextures();
	writer.BindTexture(uiElement->options.texture);
    uiElement->renderState = writer.End();
}

void ZUIElementManager::SetBorder(const ZHUIElement& handle, const ZUIBorder& border)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);

    uiElement->options.border = border;
    uiElement->uniformBuffer->Update(offsetof(ZUIUniforms, borderColor), sizeof(uiElement->options.border.color), glm::value_ptr(uiElement->options.border.color));
    uiElement->uniformBuffer->Update(offsetof(ZUIUniforms, borderWidth), sizeof(uiElement->options.border.width), &uiElement->options.border.width);
    uiElement->uniformBuffer->Update(offsetof(ZUIUniforms, borderRadius), sizeof(uiElement->options.border.radius), &uiElement->options.border.radius);
}

void ZUIElementManager::SetColor(const ZHUIElement& handle, const glm::vec4& newColor)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);

    uiElement->options.color = newColor;
    uiElement->uniformBuffer->Update(offsetof(ZUIUniforms, color), sizeof(uiElement->options.color), glm::value_ptr(uiElement->options.color));
}

void ZUIElementManager::SetOpacity(const ZHUIElement& handle, float opacity, bool relativeToAlpha)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);

    uiElement->options.opacity = opacity;
    uiElement->options.color.a = relativeToAlpha ? uiElement->options.color.a * uiElement->options.opacity : uiElement->options.opacity;
    uiElement->options.border.color.a = relativeToAlpha ? uiElement->options.border.color.a * uiElement->options.opacity : uiElement->options.opacity;
	for (auto it = uiElement->children.begin(); it != uiElement->children.end(); it++)
	{
		SetOpacity(it->second, opacity, relativeToAlpha);
	}
}

void ZUIElementManager::SetTranslationBounds(const ZHUIElement& handle, float left, float right, float bottom, float top)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);

    uiElement->options.translationBounds = glm::vec4(left, right, bottom, top);
	for (auto it = uiElement->children.begin(); it != uiElement->children.end(); it++)
	{
		SetTranslationBounds(it->second, left, right, bottom, top);
	}
}

void ZUIElementManager::SetShader(const ZHUIElement& handle, const ZHShader& shader)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);

    uiElement->options.shader = shader;
	ZRenderStateGroupWriter writer(uiElement->renderState);
	writer.Begin();
	writer.SetShader(uiElement->options.shader);
    uiElement->renderState = writer.End();
}

void ZUIElementManager::SetLayout(const ZHUIElement& handle, const std::shared_ptr<ZUILayout>& layout)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
    uiElement->options.layout = layout;
}

void ZUIElementManager::SetParent(const ZHUIElement& handle, const ZHUIElement& parent)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	uiElement->parent = parent;
}

void ZUIElementManager::SetScene(const ZHUIElement& handle, const std::shared_ptr<ZScene> scene)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	uiElement->scene = scene;
}

void ZUIElementManager::SetFlipped(const ZHUIElement& handle, bool flipped)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);

    uiElement->options.flipped = flipped;
	RecalculateProjectionMatrix(handle);
}

void ZUIElementManager::SetZOrder(const ZHUIElement& handle, unsigned int zOrder)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);

	ZRenderStateGroupWriter writer(uiElement->renderState);
	writer.Begin();
	writer.SetRenderDepth(zOrder);
    uiElement->renderState = writer.End();
}

void ZUIElementManager::ResetModelMatrix(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
    uiElement->modelMatrix = glm::mat4(1.f);
}

void ZUIElementManager::Translate(const ZHUIElement& handle, const glm::vec2& translation)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);

    uiElement->options.calculatedRect.position += translation;
	ClampToBounds(handle);
	RecalculateModelMatrix(handle);

	for (auto it = uiElement->children.begin(); it != uiElement->children.end(); it++)
	{
		Translate(it->second, translation);
	}
}

void ZUIElementManager::Rotate(const ZHUIElement& handle, float angle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);

    uiElement->options.orientation += angle;
	RecalculateModelMatrix(handle);

	for (auto it = uiElement->children.begin(); it != uiElement->children.end(); it++)
	{
		Rotate(it->second, angle);
	}
}

void ZUIElementManager::Scale(const ZHUIElement& handle, const glm::vec2& factor)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);

    uiElement->options.rect.size *= factor;
    uiElement->options.calculatedRect.size *= factor;
	ClampToSizeLimits(handle);
	RecalculateModelMatrix(handle);
}

void ZUIElementManager::Hide(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
    uiElement->options.hidden = true;
}

void ZUIElementManager::Show(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
    uiElement->options.hidden = false;
}

void ZUIElementManager::Enable(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
    uiElement->options.enabled = true;
}

void ZUIElementManager::Disable(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
    uiElement->options.enabled = false;
}

void ZUIElementManager::Select(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
    uiElement->options.selected = uiElement->options.enabled;
}

void ZUIElementManager::Deselect(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
    uiElement->options.selected = false;
}

bool ZUIElementManager::HasChildren(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
    return !uiElement->children.empty();
}

void ZUIElementManager::AddChild(const ZHUIElement& handle, const ZHUIElement& element)
{
	assert(!handle.IsNull() && !element.IsNull() && "Cannot add null ui element as child!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
    ZUIElement* childElement = resourcePool_.Get(element);
    
    uiElement->children[childElement->name] = element;

	SetOpacity(element, Opacity(handle), true);
	SetTranslationBounds(element, uiElement->options.translationBounds.x, uiElement->options.translationBounds.y, uiElement->options.translationBounds.z, uiElement->options.translationBounds.w);

	SetParent(element, handle);
	SetScene(element, Scene(handle));

	LayoutChild(handle, element);
}

void ZUIElementManager::RemoveChild(const ZHUIElement& handle, const ZHUIElement& element, bool recurse)
{
	assert(!handle.IsNull() && !element.IsNull() && "Cannot remove null ui element!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	ZUIElement* childElement = resourcePool_.Get(element);

	if (uiElement->children.find(childElement->name) != uiElement->children.end())
	{
		RemoveParent(element);
        uiElement->children.erase(childElement->name);
		if (uiElement->options.layout) {
            uiElement->options.layout->RemoveRect(childElement->name);
		}
	}

	if (recurse) {
		for (auto it = uiElement->children.begin(), end = uiElement->children.end(); it != end; it++)
		{
			RemoveChild(it->second, element, recurse);
		}
	}
}

void ZUIElementManager::DoRecursiveChildUpdate(const ZHUIElement& handle, std::function<void(ZHUIElement)> callback)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);

	for (auto it = uiElement->children.begin(); it != uiElement->children.end(); it++)
	{
		callback(it->second);
		if (HasChildren(it->second))
		{
			DoRecursiveChildUpdate(it->second, callback);
		}
	}
}

void ZUIElementManager::RemoveParent(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
    uiElement->parent = ZHUIElement();
}

std::shared_ptr<ZMesh2D> ZUIElementManager::ElementShape(const ZHUIElement& handle)
{
	static std::shared_ptr<ZMesh2D> mesh = std::make_shared<ZMesh2D>(ZUIHelpers::NewQuad());
	return mesh;
};

bool ZUIElementManager::TrySelect(const ZHUIElement& handle, const glm::vec3& position)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);

	bool selected = false;
	if (uiElement->options.enabled && Contains(handle, position))
	{
		bool selectedChild = false;
		for (auto it = uiElement->children.begin(); it != uiElement->children.end(); it++)
		{
			selectedChild = TrySelect(it->second, position);
		}

		if (!selectedChild)
		{
			std::shared_ptr<ZElementSelectedEvent> objectSelectEvent(new ZElementSelectedEvent(handle, position));
			ZServices::EventAgent()->Queue(objectSelectEvent);
		}

		selected = true;
	}
	return selected;
}

bool ZUIElementManager::Contains(const ZHUIElement& handle, const glm::vec2& point)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);
	return uiElement->options.calculatedRect.Contains(point);
}

void ZUIElementManager::LayoutChild(const ZHUIElement& handle, const ZHUIElement& child, bool force)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);

	auto scene = Scene(handle);
	if (!scene) return;

	auto paddedRect = PaddedRect(handle);
	SetRect(child, Rect(child), paddedRect);
	if (uiElement->options.layout) {
		ZRect rect = uiElement->options.layout->GetRect(Name(child), CalculatedRect(child).size, force);
		if (Positioning(child) == ZPositioning::Relative) {
			rect.position = ZUIHelpers::AbsoluteToRelativeCoords(
				rect.position,
                uiElement->options.calculatedRect.size == glm::vec2(0.f) ? scene->Domain()->Resolution() : paddedRect.size
            );
        }
        if (Scaling(child) == ZPositioning::Relative) {
            // We want to lock the relative position of the rect if it is at or past its minimum or maximum size
            ZRect childCalcRect = CalculatedRect(child);
            glm::vec2 childMaxSize = MaxSize(child);
            glm::vec2 childMinSize = MinSize(child);
            if (childCalcRect.size.x <= childMinSize.x || childCalcRect.size.y <= childMinSize.y
                || childCalcRect.size.x >= childMaxSize.x || childCalcRect.size.y >= childMaxSize.x) {
                rect.size = Rect(child).size;
            }
            else {
                rect.size = ZUIHelpers::AbsoluteToRelativeCoords(
                    rect.size,
                    uiElement->options.calculatedRect.size == glm::vec2(0.f) ? scene->Domain()->Resolution() : paddedRect.size
				);
			}
		}
		SetRect(child, rect, paddedRect);
	}

	LayoutChildren(child, true);
}

void ZUIElementManager::LayoutChildren(const ZHUIElement& handle, bool force)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);

	// If there's a layout for the given element we want to traverse the children in order of the calculated rects
	// in the layout object, otherwise we might update the rect for an element and miss updating dependent rects
	// in the layout if those objects were already traversed.
	if (uiElement->options.layout) {
		std::vector<ZHUIElement> layoutList;
		for (auto it = uiElement->options.layout->Rects().begin(); it != uiElement->options.layout->Rects().end(); it++)
		{
			auto elIt = uiElement->children.find((*it).id);
			if (elIt != uiElement->children.end())
			{
				layoutList.push_back(elIt->second);
			}
		}
		for (auto it = layoutList.begin(); it != layoutList.end(); it++)
		{
			LayoutChild(handle, (*it), force);
		}
	}
	else {
		for (auto it = uiElement->children.begin(); it != uiElement->children.end(); it++)
		{
			LayoutChild(handle, it->second, force);
		}
	}
}

void ZUIElementManager::ClampToSizeLimits(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);

	if (uiElement->options.minSize.x > 0) {
		uiElement->options.calculatedRect.size.x =
			glm::clamp(uiElement->options.calculatedRect.size.x, uiElement->options.minSize.x, std::numeric_limits<float>::max());
	}
	if (uiElement->options.minSize.y > 0) {
		uiElement->options.calculatedRect.size.y =
			glm::clamp(uiElement->options.calculatedRect.size.y, uiElement->options.minSize.y, std::numeric_limits<float>::max());
	}
	if (uiElement->options.maxSize.x > 0) {
		uiElement->options.calculatedRect.size.x =
			glm::clamp(uiElement->options.calculatedRect.size.x, 0.f, uiElement->options.maxSize.x);
	}
	if (uiElement->options.maxSize.y > 0) {
		uiElement->options.calculatedRect.size.y =
			glm::clamp(uiElement->options.calculatedRect.size.y, 0.f, uiElement->options.maxSize.y);
	}
}

void ZUIElementManager::ClampToBounds(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);

	auto scene = Scene(handle);
	if (!scene) return;

	glm::vec2 resolution = scene->Domain()->Resolution();

	uiElement->options.calculatedRect.position.x =
		glm::sign(uiElement->options.calculatedRect.position.x) *
		glm::clamp(glm::abs(uiElement->options.calculatedRect.position.x),
			uiElement->options.translationBounds.x * resolution.x,
			uiElement->options.translationBounds.y * resolution.x);
	uiElement->options.calculatedRect.position.y =
		glm::sign(uiElement->options.calculatedRect.position.y) *
		glm::clamp(glm::abs(uiElement->options.calculatedRect.position.y),
			uiElement->options.translationBounds.z * resolution.y,
			uiElement->options.translationBounds.w * resolution.y);
}

void ZUIElementManager::RecalculateRect(const ZHUIElement& handle, bool force)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);

	auto parent = Parent(handle);
	if (parent.IsNull() || !Layout(parent) || force) {
		SetRect(handle, uiElement->options.rect, parent ? PaddedRect(parent) : ZRect());
	}

	for (auto it = uiElement->children.begin(); it != uiElement->children.end(); it++) {
		LayoutChild(handle, it->second, true);
	}
}

void ZUIElementManager::RecalculateModelMatrix(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);

	// By default we want to draw with the top left as (0, 0), so we use an identity vector to flip since openGL coordinates
	// begin at the bottom left and by default textures must be flipped to orient correctly. This might
	// do better as a global vector that changes based on the graphics implementation
	glm::vec3 flipVector = uiElement->options.flipped ? glm::vec3(1.f) : glm::vec3(1.f, -1.f, 1.f);
	glm::mat4 scale = glm::scale(glm::mat4(1.f), glm::vec3(uiElement->options.calculatedRect.size * 0.5f, 0.f) * flipVector);
	glm::mat4 rotate = glm::rotate(glm::mat4(1.f), uiElement->options.orientation, glm::vec3(0.f, 0.f, 1.f));
	glm::mat4 translate = glm::translate(glm::mat4(1.f), glm::vec3(uiElement->options.calculatedRect.position + uiElement->options.calculatedRect.size * 0.5f, 0.f));
	uiElement->modelMatrix = translate * rotate * scale;

	uiElement->uniformBuffer->Update(offsetof(ZUIUniforms, M), sizeof(uiElement->modelMatrix), glm::value_ptr(uiElement->modelMatrix));
}

void ZUIElementManager::RecalculateProjectionMatrix(const ZHUIElement& handle)
{
	assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
	ZUIElement* uiElement = resourcePool_.Get(handle);

	if (auto scene = Scene(handle)) {
		glm::vec2 resolution = scene->Domain()->Resolution();
		uiElement->projectionMatrix = glm::ortho(0.f, (float)resolution.x, (float)resolution.y, 0.f, -1.f, 1.f);
		uiElement->uniformBuffer->Update(offsetof(ZUIUniforms, P), sizeof(uiElement->projectionMatrix), glm::value_ptr(uiElement->projectionMatrix));
	}
}

void ZUIElementManager::OnWindowResized(const std::shared_ptr<ZWindowResizeEvent>& event)
{
	for (auto it = loadedResources_.begin(); it != loadedResources_.end(); it++)
	{
		RecalculateProjectionMatrix(it->second);
		RecalculateRect(it->second);
	}
}