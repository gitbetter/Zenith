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
static std::shared_ptr<Type> Create();\
static std::shared_ptr<Type> Create(const ZUIElementOptions& options, const std::shared_ptr<ZScene>& scene = nullptr);

#define DEFINE_UI_CREATORS(Type)\
std::shared_ptr<Type> Type::Create()\
{\
    std::shared_ptr<Type> element = std::make_shared<Type>();\
    return element;\
}\
std::shared_ptr<Type> Type::Create(const ZUIElementOptions& options, const std::shared_ptr<ZScene>& scene)\
{\
    std::shared_ptr<Type> element = std::make_shared<Type>(options);\
    if (scene) {\
        element->SetScene(scene);\
        element->Initialize();\
    }\
    return element;\
}

// Includes
#include "ZTexture.hpp"
#include "ZProcess.hpp"
#include "ZUIHelpers.hpp"
#include "ZOFTree.hpp"

// Forward Declarations
class ZScene;
class ZMesh2D;
class ZShader;
class ZUILayout;
class ZUniformBuffer;
class ZRenderStateGroup;
class ZWindowResizeEvent;

// Class and Data Structure Definitions

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
    glm::vec4                                translationBounds{ 0.f, 1.f, 0.f, 1.f };
    glm::vec4                                color{ 0.0f };
    float                                    opacity{ 1.f };
    std::shared_ptr<ZShader>                 shader{ nullptr };
    ZTexture::ptr                            texture;
    ZUIBorder                                border;
    std::shared_ptr<ZUILayout>               layout;
};

class ZUIElement : public ZProcess, public std::enable_shared_from_this<ZUIElement>
{

    friend class ZScene;
    friend class ZUI;

public:

    ZUIElement(const glm::vec2& position = glm::vec2(0.f), const glm::vec2& scale = glm::vec2(1.f));
    ZUIElement(const ZUIElementOptions& options);
    virtual ~ZUIElement();

    virtual void                        Initialize() override;
    virtual void                        Initialize(const std::shared_ptr<ZOFNode>& root);

    virtual void                        Prepare(double deltaTime, unsigned int zOrder = 0);
    virtual unsigned int                PrepareChildren(double deltaTime, unsigned int zOrder = 0);

    ZUIElementType                      Type() const { return type_; }
    bool                                Enabled() const { return options_.enabled; }
    bool                                Hidden() const { return  options_.hidden; }
    bool                                Selected() const { return  options_.selected; }
    bool                                Flipped() const { return options_.flipped; }
    ZPositioning                        Positioning() const { return options_.positioning; }
    ZPositioning                        Scaling() const { return options_.scaling; }
    glm::vec2                           Position() const { return options_.rect.position; }
    glm::vec2                           Size() const { return options_.rect.size; }
    ZRect                               Rect() const { return options_.rect; }
    ZRect                               CalculatedRect() const { return options_.calculatedRect; }
    float                               Angle() const { return options_.orientation; }
    glm::vec4                           Color() const { return  options_.color; }
    float                               Opacity() const { return  options_.opacity; }
    glm::vec4                           TranslationBounds() const { return  options_.translationBounds; }
    const ZTexture::ptr&                Texture() const { return  options_.texture; }
    const ZUIBorder&                    Border() const { return options_.border; }
    const std::shared_ptr<ZShader>&     Shader() const { return  options_.shader; }
    const ZUIElementMap&                Children() const { return children_; }
    const std::shared_ptr<ZUILayout>&   Layout() const { return options_.layout; }
    glm::mat4                           ModelMatrix() const { return modelMatrix_; }
    glm::mat4                           ProjectionMatrix() const { return projectionMatrix_; }
    ZUIElementOptions                   Options() const { return options_; }
    std::shared_ptr<ZUIElement>         Parent() const;
    std::shared_ptr<ZScene>             Scene() const;

    void                                SetPositioning(const ZPositioning& positioning) { options_.positioning = positioning; }
    void                                SetScaling(const ZPositioning& scaling) { options_.scaling = scaling; }
    void                                SetRect(const ZRect& rect, const ZRect& relativeTo = ZRect());
    void                                SetSize(const glm::vec2& size, const ZRect& relativeTo = ZRect());
    void                                SetPosition(const glm::vec2& position, const ZRect& relativeTo = ZRect());
    void                                SetRotation(float angle);
    void                                SetTexture(const ZTexture::ptr& texture);
    void                                SetBorder(const ZUIBorder& border);
    virtual void                        SetColor(const glm::vec4& newColor);
    void                                SetOpacity(float opacity, bool relativeToAlpha = false);
    void                                SetTranslationBounds(float left, float right, float bottom, float top);
    void                                SetShader(const std::shared_ptr<ZShader>& shader) { options_.shader = shader; }
    void                                SetLayout(const std::shared_ptr<ZUILayout>& layout) { options_.layout = layout; }
    void                                SetParent(std::shared_ptr<ZUIElement> parent) { parent_ = parent; }
    void                                SetScene(const std::shared_ptr<ZScene> scene) { scene_ = scene; }
    void                                SetFlipped(bool flipped);
    void                                SetZOrder(unsigned int zOrder);

    void                                ResetModelMatrix() { modelMatrix_ = glm::mat4(1.f); }
    void                                Translate(const glm::vec2& translation);
    void                                Rotate(float angle);
    void                                Scale(const glm::vec2& factor);

    void                                Hide() { options_.hidden = true; }
    void                                Show() { options_.hidden = false; }
    void                                Enable() { options_.enabled = true; }
    void                                Disable() { options_.enabled = false; }
    void                                Select() { options_.selected = options_.enabled; }
    void                                Deselect() { options_.selected = false; }

    bool                                HasChildren() const { return !children_.empty(); }
    virtual void                        AddChild(const std::shared_ptr<ZUIElement>& element);
    void                                RemoveChild(const std::shared_ptr<ZUIElement>& element, bool recurse = false);
    void                                DoRecursiveChildUpdate(std::function<void(std::shared_ptr<ZUIElement>)> callback);
    void                                RemoveParent();

    virtual std::shared_ptr<ZMesh2D>    ElementShape();

    bool                                TrySelect(const glm::vec3& position);
    bool                                Contains(const glm::vec2& point);

    void                                CleanUp() override;

    template<class T>
    std::shared_ptr<T> Child(const std::string& id)
    {
        if (!std::is_base_of<ZUIElement, T>::value) return nullptr;

        std::shared_ptr<T> el;
        for (auto it = children_.begin(); it != children_.end(); it++)
        {
            if ((el = std::dynamic_pointer_cast<T>(it->second)) && zenith::strings::HasSuffix(it->first, id))
            {
                return el;
            }
            el = nullptr;
        }

        return el;
    }

    static ZUIElementMap                Load(std::shared_ptr<ZOFTree> data, const std::shared_ptr<ZScene>& scene);
    static std::shared_ptr<ZUIElement>  Create(const std::string& type);

protected:

    bool                                     dirty_;
    bool                                     renderable_;
    std::weak_ptr<ZUIElement>                parent_;
    std::weak_ptr<ZScene>                    scene_;
    glm::mat4                                modelMatrix_;
    glm::mat4                                projectionMatrix_;
    ZUIElementMap                            children_;
    ZUIElementType                           type_;
    ZUIElementOptions                        options_;

    std::shared_ptr<ZUniformBuffer>          uniformBuffer_;
    std::shared_ptr<ZRenderStateGroup>       renderState_;
    
    static ZIDSequence                       idGenerator_;

    virtual void                        OnRectChanged() { };

    void                                LayoutChild(const std::shared_ptr<ZUIElement>& element, bool force = false);
    void                                LayoutChildren(const std::shared_ptr<ZUIElement>& element, bool force = false);

private:

    void                                ClampToSizeLimits();
    void                                ClampToBounds();
    void                                RecalculateModelMatrix();
    void                                RecalculateProjectionMatrix();
    void                                OnWindowResized(const std::shared_ptr<ZWindowResizeEvent>& event);

};