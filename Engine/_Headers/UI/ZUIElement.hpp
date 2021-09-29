/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZUIElement.hpp

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

#pragma once

#define DECLARE_UI_CREATORS(Type)\
static ZHUIElement Create();\
static ZHUIElement Create(const ZUIElementOptions& options, const std::shared_ptr<ZScene>& scene = nullptr);

#define DEFINE_UI_CREATORS(Type)\
ZHUIElement Type::Create()\
{\
	ZHUIElement handle;
	Type* element = resourcePool_.New<Type>(handle);
	return handle
}\
ZHUIElement Type::Create(const ZUIElementOptions& options, const std::shared_ptr<ZScene>& scene)\
{\
	ZHUIElement handle;
	Type* element = resourcePool_.New(handle);
    if (scene) {\
        SetScene(handle, scene);\
        Initialize(handle);\
    }\
    return element;\
}

#include "ZUILayout.hpp"
#include "ZUIHelpers.hpp"
#include "ZOFTree.hpp"

class ZScene;
class ZMesh2D;
class ZUniformBuffer;
class ZRenderStateGroup;
class ZWindowResizeEvent;

enum class ZUIElementType
{
	Canvas, Panel, Button, CheckBox, Image, InputField, LabeledElement, ListPanel, Text
};

struct ZUIBorder
{
    float width{ 0.f };
    float radius{ 0.f };
    glm::vec4 color{ 0.f };

    ZUIBorder() 
    { }
    ZUIBorder(const glm::vec4& color, float width, float radius)
        : width(width), radius(radius), color(color)
    { }
};

struct ZUIElementOptions
{
    bool                                     hidden{ false };
    bool                                     enabled{ true };
    bool                                     selected{ false };
    bool                                     flipped{ false };
    ZPositioning                             positioning{ ZPositioning::Absolute };
    ZPositioning                             scaling{ ZPositioning::Absolute };
    ZRect                                    calculatedRect{ 0.f, 0.f, 1.f, 1.f };
    ZRect                                    rect{ 0.f, 0.f, 0.f, 0.f };
    float                                    orientation{ 0.f };
    glm::vec2                                maxSize{ 0.f };
    glm::vec2                                minSize{ 0.f };
    glm::vec2                                padding{ 0.f };
    glm::vec4                                translationBounds{ 0.f, 1.f, 0.f, 1.f };
    glm::vec4                                color{ 0.0f };
    float                                    opacity{ 1.f };
    ZHShader                                 shader;
    ZHTexture                                texture;
    ZUIBorder                                border;
    std::shared_ptr<ZUILayout>               layout;
};

struct ZUIElement
{

	ZHUIElement								 handle;
    ZHUIElement                              parent;
    std::weak_ptr<ZScene>                    scene;
    glm::mat4                                modelMatrix;
    glm::mat4                                projectionMatrix;
    ZUIElementMap                            children;
    ZUIElementType                           type;
    ZUIElementOptions                        options;
    std::string                              name;

    std::shared_ptr<ZUniformBuffer>          uniformBuffer;
    std::shared_ptr<ZRenderStateGroup>       renderState;

	ZUIElement();

public:
    
    virtual void                             OnRectChanged() { };

private:

    static ZIDSequence                       idGenerator_;

};

class ZUIElementManager : public ZResourceManager<ZUIElement, ZHUIElement>
{
public:

	virtual ~ZUIElementManager() = default;

public:

	virtual void Initialize() override;
	virtual void CleanUp() override;

public:

	void							    Initialize(const ZHUIElement& handle);

	virtual ZHUIElement                 Deserialize(const ZOFHandle& dataHandle, const std::shared_ptr<ZOFObjectNode>& dataNode, const std::shared_ptr<ZScene>& scene);
	ZUIElementList                      Deserialize(std::shared_ptr<ZOFNode> data, const std::shared_ptr<ZScene>& scene);
	ZHUIElement				            Create(const std::string& type);

	virtual void                        Prepare(const ZHUIElement& handle, double deltaTime, unsigned int zOrder = 0);
	virtual unsigned int                PrepareChildren(const ZHUIElement& handle, double deltaTime, unsigned int zOrder = 0);

	std::string						    Name(const ZHUIElement& handle);
	ZUIElementType                      Type(const ZHUIElement& handle);
	bool                                Enabled(const ZHUIElement& handle);
	bool                                Hidden(const ZHUIElement& handle);
	bool                                Selected(const ZHUIElement& handle);
	bool                                Flipped(const ZHUIElement& handle);
	ZPositioning                        Positioning(const ZHUIElement& handle);
	ZPositioning                        Scaling(const ZHUIElement& handle);
	glm::vec2                           Position(const ZHUIElement& handle);
	glm::vec2                           Size(const ZHUIElement& handle);
	glm::vec2                           MaxSize(const ZHUIElement& handle);
	glm::vec2                           MinSize(const ZHUIElement& handle);
	ZRect                               Rect(const ZHUIElement& handle);
	ZRect                               CalculatedRect(const ZHUIElement& handle);
	glm::vec2                           Padding(const ZHUIElement& handle);
	float                               Angle(const ZHUIElement& handle);
	glm::vec4                           Color(const ZHUIElement& handle);
	float                               Opacity(const ZHUIElement& handle);
	glm::vec4                           TranslationBounds(const ZHUIElement& handle);
	const ZHTexture&                    Texture(const ZHUIElement& handle);
	const ZUIBorder&                    Border(const ZHUIElement& handle);
	const ZHShader&                     Shader(const ZHUIElement& handle);
	const ZUIElementMap&                Children(const ZHUIElement& handle);
	const std::shared_ptr<ZUILayout>&   Layout(const ZHUIElement& handle);
	glm::mat4                           ModelMatrix(const ZHUIElement& handle);
	glm::mat4                           ProjectionMatrix(const ZHUIElement& handle);
	ZUIElementOptions                   Options(const ZHUIElement& handle);
	ZRect                               PaddedRect(const ZHUIElement& handle);
	ZHUIElement                         Parent(const ZHUIElement& handle);
	std::shared_ptr<ZScene>             Scene(const ZHUIElement& handle);

	void                                SetPositioning(const ZHUIElement& handle, const ZPositioning& positioning);
	void                                SetScaling(const ZHUIElement& handle, const ZPositioning& scaling);
	void                                SetPadding(const ZHUIElement& handle, const glm::vec2& padding);
	void                                SetRect(const ZHUIElement& handle, const ZRect& rect, const ZRect& relativeTo = ZRect());
	void                                SetSize(const ZHUIElement& handle, const glm::vec2& size, const ZRect& relativeTo = ZRect());
	void                                SetMaxSize(const ZHUIElement& handle, const glm::vec2& size);
	void                                SetPosition(const ZHUIElement& handle, const glm::vec2& position, const ZRect& relativeTo = ZRect());
	void                                SetRotation(const ZHUIElement& handle, float angle);
	void                                SetTexture(const ZHUIElement& handle, const ZHTexture& texture);
	void                                SetBorder(const ZHUIElement& handle, const ZUIBorder& border);
	virtual void                        SetColor(const ZHUIElement& handle, const glm::vec4& newColor);
	void                                SetOpacity(const ZHUIElement& handle, float opacity, bool relativeToAlpha = false);
	void                                SetTranslationBounds(const ZHUIElement& handle, float left, float right, float bottom, float top);
	void                                SetShader(const ZHUIElement& handle, const ZHShader& shader);
	void                                SetLayout(const ZHUIElement& handle, const std::shared_ptr<ZUILayout>& layout);
	void                                SetParent(const ZHUIElement& handle, const ZHUIElement& parent);
	void                                SetScene(const ZHUIElement& handle, const std::shared_ptr<ZScene> scene);
	void                                SetFlipped(const ZHUIElement& handle, bool flipped);
	void                                SetZOrder(const ZHUIElement& handle, unsigned int zOrder);

	void                                ResetModelMatrix(const ZHUIElement& handle);
	void                                Translate(const ZHUIElement& handle, const glm::vec2& translation);
	void                                Rotate(const ZHUIElement& handle, float angle);
	void                                Scale(const ZHUIElement& handle, const glm::vec2& factor);

	void                                Hide(const ZHUIElement& handle);
	void                                Show(const ZHUIElement& handle);
	void                                Enable(const ZHUIElement& handle);
	void                                Disable(const ZHUIElement& handle);
	void                                Select(const ZHUIElement& handle);
	void                                Deselect(const ZHUIElement& handle);

	bool                                HasChildren(const ZHUIElement& handle);
	virtual void                        AddChild(const ZHUIElement& handle, const ZHUIElement& element);
	void                                RemoveChild(const ZHUIElement& handle, const ZHUIElement& element, bool recurse = false);
	void                                DoRecursiveChildUpdate(const ZHUIElement& handle, std::function<void(ZHUIElement)> callback);
	void                                RemoveParent(const ZHUIElement& handle);

	virtual std::shared_ptr<ZMesh2D>    ElementShape(const ZHUIElement& handle);

	bool                                TrySelect(const ZHUIElement& handle, const glm::vec3& position);
	bool                                Contains(const ZHUIElement& handle, const glm::vec2& point);

	template<class T>
	ZHUIElement ChildByName(const ZHUIElement& handle, const std::string& name)
	{
		assert(!handle.IsNull() && "Cannot fetch property with a null ui element handle!");
		ZUIElement* uiElement = resourcePool_.Get(handle);

		if (!std::is_base_of<ZUIElement, T>::value) return nullptr;

		for (auto it = uiElement->children.begin(); it != uiElement->children.end(); it++)
		{
			ZUIElement* childElement = resourcePool_.Get(it->second);
			if ((dynamic_cast<T*>(childElement)) && zenith::strings::HasSuffix(it->first, name))
			{
				return it->second;
			}
		}

		return ZHUIElement();
	}

protected:

	void                                LayoutChild(const ZHUIElement& handle, const ZHUIElement& child, bool force = false);
	void                                LayoutChildren(const ZHUIElement& handle, bool force = false);

private:

	void                                ClampToSizeLimits(const ZHUIElement& handle);
	void                                ClampToBounds(const ZHUIElement& handle);
	void                                RecalculateRect(const ZHUIElement& handle, bool force = false);
	void                                RecalculateModelMatrix(const ZHUIElement& handle);
	void                                RecalculateProjectionMatrix(const ZHUIElement& handle);

private:

	void                                OnWindowResized(const std::shared_ptr<ZWindowResizeEvent>& event);

};