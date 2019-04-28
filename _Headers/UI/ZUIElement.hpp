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

// Forward Declarations
class ZMesh2D;
class ZShader;

// Class and Data Structure Definitions

struct ZUIBorder {
    float width = 0.f;
    glm::vec4 color = glm::vec4(1.f);
};

class ZUIElement : public ZProcess {
    
private:
    
    void ClampToBounds();
    
public:
    
    ZUIElement(glm::vec2 position = glm::vec2(0.f), glm::vec2 scale = glm::vec2(1.f));
    virtual ~ZUIElement() { }
    
    virtual void Initialize() override { }
    virtual void Initialize(std::shared_ptr<ZOFNode> root);
    
    bool Enabled() { return enabled_; }
    bool Hidden() { return hidden_; }
    bool Selected() { return selected_; }
    glm::vec3 Position();
    glm::vec3 Size();
    float Angle();
    glm::vec4 Color() { return color_; }
    const ZTexture& Texture() { return texture_; }
    const std::shared_ptr<ZShader>& Shader() const { return shader_; }
    
    void SetSize(glm::vec2 size);
    void SetPosition(glm::vec2 position);
    void SetRotation(float angle);
    
    void SetTexture(ZTexture texture) { texture_ = texture; }
    virtual void SetColor(glm::vec4 newColor) { color_ = newColor; }
    void SetTranslationBounds(float left, float right, float bottom, float top);
    void SetShader(std::shared_ptr<ZShader> shader) { shader_ = shader; }
    void ResetModelMatrix() { modelMatrix_ = glm::mat4(1.f); }
    
    void Translate(glm::vec2 translation);
    void Rotate(float angle);
    void Scale(glm::vec2 factor);
    
    void Hide() { hidden_ = true; }
    void Show() { hidden_ = false; }
    void Enable() { enabled_ = true; }
    void Disable() { enabled_ = false; }
    void Select() { if (enabled_) selected_ = true; }
    void Deselect() { selected_ = false; }
    
    virtual void AddChild(std::shared_ptr<ZUIElement> element);
    bool RemoveChild(std::shared_ptr<ZUIElement> element);
    
    void SetParent(ZUIElement* parent) { parent_ = parent; }
    void RemoveParent();
    
    virtual void Render(float frameMix = 1.f, RENDER_OP renderOp = RENDER_OP_COLOR);
    virtual ZMesh2D ElementShape();
    
    void RenderChildren();
    
    bool TrySelect(glm::vec3 position);
    bool Contains(glm::vec3 point);
    
    void CleanUp() override;
    
    ZUIElement* Parent() const { return parent_; }
    
    template<class T>
    std::shared_ptr<T> Child(std::string id) {
        if(!std::is_base_of<ZUIElement, T>::value) return nullptr;
        
        std::shared_ptr<T> el;
        for (auto it = children_.begin(); it != children_.end(); it++) {
            if ((el = std::dynamic_pointer_cast<T>(*it)) && el->ID() == id) return el;
            el = nullptr;
        }
        
        return el;
    }
    
protected:
    
    bool hidden_, enabled_, selected_, dirty_;
    ZUIElement* parent_ = nullptr;
    glm::mat4 modelMatrix_;
    glm::vec4 translationBounds_;
    glm::vec4 color_;
    std::vector<std::shared_ptr<ZUIElement>> children_;
    std::shared_ptr<ZShader> shader_;
    ZTexture texture_;
    ZUIBorder border_;
    
};
