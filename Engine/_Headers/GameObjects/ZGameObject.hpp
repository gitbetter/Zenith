/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

   ZGameObject.hpp

   Created by Adrian Sanchez on 1/25/19.
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
#include "ZProcess.hpp"
#include "ZGraphicsComponent.hpp"
#include "ZProcessRunner.hpp"
#include "ZStringHelpers.hpp"

// Forward Declarations
class ZGame;
class ZScene;
class ZGOFactory;
struct ZOFNode;

// Class Definitions
struct ZGameObjectProperties
{
    ZRenderPass renderPass;
    glm::vec4 position, previousPosition;
    glm::vec3 scale, previousScale;
    glm::quat orientation, previousOrientation;
    glm::mat4 modelMatrix;
    std::string name;
};

class ZGameObject : public ZProcess, public std::enable_shared_from_this<ZGameObject>
{

    friend class ZScene;
    friend class ZSceneRoot;
    friend class ZGOFactory;

public:

    ZGameObject(const glm::vec3& position = glm::vec3(0.f, 1.f, 0.f), const glm::quat& orientation = glm::quat(glm::vec3(0.f)));
    ZGameObject(const std::string& name) : ZGameObject() { properties_.name = name; }
    virtual ~ZGameObject() {}

    virtual void Initialize() override { ZProcess::Initialize(); }
    virtual void Initialize(std::shared_ptr<ZOFNode> root);

    virtual void PreRender();
    virtual void Render(ZRenderOp renderOp = ZRenderOp::Color);
    virtual void RenderChildren(ZRenderOp renderOp = ZRenderOp::Color);
    virtual void PostRender();

    void CalculateDerivedData();

    virtual std::shared_ptr<ZGameObject> Clone();
    virtual void AddChild(std::shared_ptr<ZGameObject> gameObject);
    virtual void RemoveChild(std::shared_ptr<ZGameObject> gameObject, bool recurse = false);
    bool HasChildren() { return !children_.empty(); }
    virtual bool IsVisible();

    virtual void Destroy();

    ZScene* Scene() const { return scene_; }
    std::string Name() const { return properties_.name; }
    ZRenderPass RenderPass() const { return properties_.renderPass; }
    ZGameObject* Parent() { return parent_; }
    virtual ZGameObjectList& Children() { return children_; }
    glm::vec3 Position();
    glm::vec3 Scale();
    glm::quat Orientation();
    glm::vec3 Front();
    glm::vec3 Up();
    glm::vec3 Right();
    glm::mat4 ModelMatrix();
    glm::vec3 PreviousPosition();
    glm::vec3 PreviousFront();
    glm::vec3 PreviousUp();
    glm::vec3 PreviousRight();

    void SetPosition(const glm::vec3& position);
    void SetScale(const glm::vec3& scale);
    void SetOrientation(const glm::quat& quaternion);
    void SetOrientation(const glm::vec3& euler);
    void SetModelMatrix(const glm::mat4& modelMatrix);
    void SetRenderPass(ZRenderPass renderPass) { properties_.renderPass = renderPass; }
    void SetName(const std::string& name) { properties_.name = name; }

    template<class T>
    typename std::enable_if<std::is_base_of<ZComponent, T>::value>::type
        AddComponent(std::shared_ptr<T> component)
    {
        std::shared_ptr<T> foundComponent = FindComponent<T>();
        if (foundComponent == nullptr)
        {
            component->object_ = this;
            components_.push_back(component);
        }
    }

    template<class T>
    std::shared_ptr<T> RemoveComponent()
    {
        ZComponentList::iterator found;
        for (ZComponentList::iterator it = components_.begin(); it != components_.end(); ++it)
        {
            if (std::dynamic_pointer_cast<T>(*it))
            {
                found = it; break;
            }
        }

        if (found == components_.end()) return nullptr;

        std::shared_ptr<T> removed = *found;
        components_.erase(found);
        return removed;
    }

    template<class T>
    std::shared_ptr<T> FindComponent()
    {
        for (std::shared_ptr<ZComponent> comp : components_)
        {
            if (std::dynamic_pointer_cast<T>(comp)) return std::dynamic_pointer_cast<T>(comp);
        }
        return nullptr;
    }

    template<class T>
    std::shared_ptr<T> Child(const std::string& id)
    {
        if (!std::is_base_of<ZGameObject, T>::value) return nullptr;

        std::shared_ptr<T> go;
        for (auto it = children_.begin(); it != children_.end(); it++)
        {
            if ((go = std::dynamic_pointer_cast<T>(*it)) && zenith::strings::HasSuffix(go->ID(), id))
            {
                return go;
            }
            go = nullptr;
        }

        return go;
    }

protected:

    ZScene* scene_ = nullptr;
    ZGameObject* parent_ = nullptr;
    ZComponentList components_;
    ZGameObjectList children_;
    ZGameObjectProperties properties_;

    struct
    {
        std::mutex position;
        std::mutex orientation;
        std::mutex scale;
        std::mutex modelMatrix;
    } objectMutexes_;

};
