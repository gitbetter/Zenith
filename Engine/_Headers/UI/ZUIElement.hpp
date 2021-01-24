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

// Includes
#include "ZMesh2D.hpp"
#include "ZProcess.hpp"
#include "ZStringHelpers.hpp"
#include "ZUIHelpers.hpp"

// Forward Declarations
class ZScene;
class ZMesh2D;
class ZShader;
class ZUILayout;

// Class and Data Structure Definitions

struct ZUIBorder
{
    float width{ 0.f };
    float radius{ 0.f };
    glm::vec4 color{ 0.f };
};

struct ZUIElementOptions
{
    bool                                     hidden{ false };
    bool                                     enabled{ true };
    bool                                     selected{ false };
    ZPositioning                             positioning{ ZPositioning::Absolute };
    ZRect                                    calculatedRect{ 0.f, 0.f, 1.f, 1.f };
    float                                    orientation{ 0.f };
    ZRect                                    rect{ 0.f, 0.f, 0.f, 0.f };
    glm::vec4                                translationBounds{ 0.f, 1.f, 0.f, 1.f };
    glm::vec4                                color{ 0.0f };
    float                                    opacity{ 1.f };
    std::shared_ptr<ZShader>                 shader{ nullptr };
    ZTexture                                 texture;
    ZUIBorder                                border;
    std::shared_ptr<ZUILayout>               layout;
};

class ZUIElement : public ZProcess, public std::enable_shared_from_this<ZUIElement>
{

    friend class ZScene;
    friend class ZUI;

private:

    void ClampToBounds();
    void RecalculateModelMatrix();
    void LayoutChild(const std::shared_ptr<ZUIElement>& element, bool force = false);
    void OnWindowResized(const std::shared_ptr<ZEvent>& event);

public:

    ZUIElement(const glm::vec2& position = glm::vec2(0.f), const glm::vec2& scale = glm::vec2(1.f));
    ZUIElement(const ZUIElementOptions& options);
    virtual ~ZUIElement() {}

    virtual void Initialize() override;
    virtual void Initialize(const std::shared_ptr<ZOFNode>& root);

    bool Enabled() const { return options_.enabled; }
    bool Hidden() const { return  options_.hidden; }
    bool Selected() const { return  options_.selected; }
    ZPositioning Positioning() const { return options_.positioning; }
    glm::vec2 Position() const { return options_.rect.position; }
    glm::vec2 Size() const { return options_.rect.size; }
    ZRect Rect() const { return options_.rect; }
    ZRect CalculatedRect() const { return options_.calculatedRect; }
    float Angle() const { return options_.orientation; }
    glm::vec4 Color() const { return  options_.color; }
    float Opacity() const { return  options_.opacity; }
    glm::vec4 TranslationBounds() const { return  options_.translationBounds; }
    const ZTexture& Texture() const { return  options_.texture; }
    const ZUIBorder& Border() const { return options_.border; }
    const std::shared_ptr<ZShader>& Shader() const { return  options_.shader; }
    const ZUIElementMap& Children() const { return children_; }
    const std::shared_ptr<ZUILayout>& Layout() const { return options_.layout; }

    void SetPositioning(const ZPositioning& positioning) { options_.positioning = positioning; }
    void SetRect(const ZRect& rect, const ZRect& relativeTo = ZRect());
    void SetSize(const glm::vec2& size, const ZRect& relativeTo = ZRect());
    void SetPosition(const glm::vec2& position, const ZRect& relativeTo = ZRect());
    void SetRotation(float angle);
    void SetTexture(const ZTexture& texture) { options_.texture = texture; }
    void SetBorder(const ZUIBorder& border) { options_.border = border; }
    virtual void SetColor(const glm::vec4& newColor) { options_.color = newColor; }
    void SetOpacity(float opacity, bool relativeToAlpha = false);
    void SetTranslationBounds(float left, float right, float bottom, float top);
    void SetShader(const std::shared_ptr<ZShader>& shader) { options_.shader = shader; }
    void SetLayout(const std::shared_ptr<ZUILayout>& layout) { options_.layout = layout; }

    void ResetModelMatrix() { modelMatrix_ = glm::mat4(1.f); }
    void Translate(const glm::vec2& translation);
    void Rotate(float angle);
    void Scale(const glm::vec2& factor);

    void Hide() { options_.hidden = true; }
    void Show() { options_.hidden = false; }
    void Enable() { options_.enabled = true; }
    void Disable() { options_.enabled = false; }
    void Select() { options_.selected = options_.enabled; }
    void Deselect() { options_.selected = false; }

    bool HasChildren() const { return !children_.empty(); }
    virtual void AddChild(const std::shared_ptr<ZUIElement>& element);
    bool RemoveChild(const std::shared_ptr<ZUIElement>& element);
    void DoRecursiveChildUpdate(std::function<void(std::shared_ptr<ZUIElement>)> callback);

    void SetParent(ZUIElement* parent) { parent_ = parent; }
    void RemoveParent();

    virtual std::shared_ptr<ZMesh2D> ElementShape();

    bool TrySelect(const glm::vec3& position);
    bool Contains(const glm::vec3& point);

    void CleanUp() override;

    ZUIElement* Parent() const { return parent_; }

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

protected:

    bool                                     dirty_;
    ZUIElement*                              parent_ = nullptr;
    ZScene*                                  scene_ = nullptr;
    glm::mat4                                modelMatrix_;
    ZUIElementMap                            children_;
    ZUIElementType                           type_;
    ZUIElementOptions                        options_;

};
