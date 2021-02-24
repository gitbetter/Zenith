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

#define DECLARE_OBJECT_CREATORS(Type)\
static std::shared_ptr<Type> Create();\
static std::shared_ptr<Type> Create(const glm::vec3& position, const glm::quat& orientation = glm::quat(glm::vec3(0.f)), const glm::vec3& scale = glm::vec3(1.f), const std::shared_ptr<ZScene>& scene = nullptr);\
static std::shared_ptr<Type> Create(const std::shared_ptr<ZOFNode>& root, const std::shared_ptr<ZScene>& scene = nullptr);

#define DEFINE_OBJECT_CREATORS(Type)\
std::shared_ptr<Type> Type::Create()\
{\
    std::shared_ptr<Type> obj = std::make_shared<Type>();\
    return obj;\
}\
std::shared_ptr<Type> Type::Create(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale, const std::shared_ptr<ZScene>& scene)\
{\
    std::shared_ptr<Type> obj = std::make_shared<Type>(position, orientation, scale);\
    if (scene) {\
        obj->scene_ = scene;\
    }\
    obj->Initialize();\
    return obj;\
}\
std::shared_ptr<Type> Type::Create(const std::shared_ptr<ZOFNode>& root, const std::shared_ptr<ZScene>& scene)\
{\
    std::shared_ptr<Type> obj = std::make_shared<Type>();\
    if (scene) {\
        obj->scene_ = scene;\
    }\
    obj->Initialize(root);\
    return obj;\
}

// Includes
#include "ZProcess.hpp"
#include "ZRenderable.hpp"
#include "ZProcessRunner.hpp"
#include "ZOFTree.hpp"

// Forward Declarations
class ZGame;
class ZScene;
class ZSkybox;
class ZGrass;

// Class Definitions
struct ZGameObjectProperties
{
    ZRenderOrder renderOrder;
    glm::vec4 position, previousPosition;
    glm::vec3 scale, previousScale;
    glm::quat orientation, previousOrientation;
    glm::mat4 localModelMatrix, modelMatrix;
    std::string name;
    bool active = true;
};

class ZGameObject : public ZProcess, public ZRenderable, public std::enable_shared_from_this<ZGameObject>
{

    friend class ZScene;
    friend class ZSceneRoot;
    friend class ZGOFactory;

public:

    ZGameObject(const glm::vec3& position = glm::vec3(0.f, 0.f, 0.f), const glm::quat& orientation = glm::quat(glm::vec3(0.f)), const glm::vec3& scale = glm::vec3(1.f));
    ZGameObject(const std::string& name) : ZGameObject() { properties_.name = name; }
    virtual ~ZGameObject() {}

    virtual void Initialize() override { ZProcess::Initialize(); }
    virtual void Initialize(std::shared_ptr<ZOFNode> root);

    virtual void PreRender() { }
    virtual void Render(double deltaTime, const std::shared_ptr<ZShader>& shader, ZRenderOp renderOp = ZRenderOp::Color) override;
    virtual void RenderChildren(double deltaTime, const std::shared_ptr<ZShader>& shader, ZRenderOp renderOp = ZRenderOp::Color);
    virtual void PostRender() { }
    virtual bool Renderable() override { return true; }

    void CalculateDerivedData();
    virtual std::shared_ptr<ZGameObject> Clone();
    virtual void AddChild(std::shared_ptr<ZGameObject> gameObject);
    virtual void RemoveChild(std::shared_ptr<ZGameObject> gameObject, bool recurse = false);
    bool HasChildren() { return !children_.empty(); }
    virtual bool IsVisible();
    virtual void Destroy();

    std::shared_ptr<ZScene> Scene() const;
    std::shared_ptr<ZGameObject> Parent() const;
    std::string Name() const { return properties_.name; }
    ZRenderOrder RenderOrder() const { return properties_.renderOrder; }
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

    void SetScene(const std::shared_ptr<ZScene>& scene);
    void SetPosition(const glm::vec3& position);
    void SetScale(const glm::vec3& scale);
    void SetOrientation(const glm::quat& quaternion);
    void SetOrientation(const glm::vec3& euler);
    void SetLocalModelMatrix(const glm::mat4& modelMatrix);
    void SetModelMatrix(const glm::mat4& modelMatrix);
    void SetRenderOrder(ZRenderOrder renderOrder) { properties_.renderOrder = renderOrder; }
    void SetName(const std::string& name) { properties_.name = name; }
    void SetActive(bool active = true);

    template<class T>
    typename std::enable_if<std::is_base_of<ZComponent, T>::value>::type
        AddComponent(std::shared_ptr<T> component)
    {
        std::shared_ptr<T> foundComponent = FindComponent<T>();
        if (foundComponent == nullptr || is_multiple_components_supported<T>::value)
        {
            component->object_ = this;
            components_.push_back(component);
        }
    }

    template<class T>
    std::shared_ptr<T> RemoveComponent(const std::string& id = "")
    {
        std::shared_ptr<T> removed = nullptr;
        if (removed = FindComponent<T>(id)) {
            components_.erase(found);
        }
        return removed;
    }

    template<class T>
    std::shared_ptr<T> FindComponent(const std::string& id = "")
    {
        ZComponentList::iterator found;
        if (is_multiple_components_supported<T>::value && !id.empty()) {
            found = std::find_if(components_.begin(), components_.end(), [&id](const auto& comp) {
                return std::dynamic_pointer_cast<T>(comp) != nullptr && id == comp->ID();
            });
        }
        else {
            found = std::find_if(components_.begin(), components_.end(), [&id](const auto& comp) {
                return std::dynamic_pointer_cast<T>(comp) != nullptr;
            });
        }
        if (found != components_.end())
            return std::dynamic_pointer_cast<T>(*found);
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

    static ZGameObjectMap Load(std::shared_ptr<ZOFTree> data, const std::shared_ptr<ZScene>& scene);

    DECLARE_OBJECT_CREATORS(ZGameObject)

protected:

    std::weak_ptr<ZScene> scene_;
    std::weak_ptr<ZGameObject> parent_;
    ZComponentList components_;
    ZGameObjectList children_;
    ZGameObjectProperties properties_;

    struct
    {
        std::mutex position;
        std::mutex orientation;
        std::mutex scale;
        std::mutex modelMatrix;
        std::mutex localModelMatrix;
    } objectMutexes_;

    static ZIDSequence idGenerator_;

};
