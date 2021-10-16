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
ZHGameObject Create();\
ZHGameObject Create(const glm::vec3& position, const glm::quat& orientation = glm::quat(glm::vec3(0.f)), const glm::vec3& scale = glm::vec3(1.f), const std::shared_ptr<ZScene>& scene = nullptr);\
ZHGameObject Create(const std::shared_ptr<ZOFObjectNode>& root, const std::shared_ptr<ZScene>& scene = nullptr);

#define DEFINE_OBJECT_CREATORS(Type)\
ZHGameObject Type::Create()\
{\
	ZHGameObject handle;\
    Type* obj = resourcePool_.New<Type>(handle);\
    return handle;\
}\
ZHGameObject Type::Create(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale, const std::shared_ptr<ZScene>& scene)\
{\
	ZHGameObject handle;\
    Type* obj = resourcePool_.New<Type>(handle);\
    if (scene) {\
        obj->scene = scene;\
    }\
    obj->OnCreate();\
    return handle;\
}\
ZHGameObject Type::Create(const std::shared_ptr<ZOFObjectNode>& root, const std::shared_ptr<ZScene>& scene)\
{\
	ZHGameObject handle;\
    Type* obj = resourcePool_.New<Type>(handle);\
    if (scene) {\
        obj->scene = scene;\
    }\
    obj->OnDeserialize(root);\
    return handle;\
}

#include "ZProcess.hpp"
#include "ZProcessRunner.hpp"
#include "ZOFTree.hpp"

class ZGame;
class ZScene;
class ZSkybox;
class ZGrass;
class ZUniformBuffer;
class ZRenderStateGroup;

struct ZGameObjectProperties
{
    ZRenderLayer renderOrder{ ZRenderLayer::Static };
    glm::vec3 position, previousPosition;
    glm::vec3 scale, previousScale;
    glm::quat orientation, previousOrientation;
    glm::mat4 localModelMatrix, modelMatrix;
    std::string name;
    bool active = true;
};

struct ZGameObject
{
    
    ZGameObject();

    std::string name;
	std::weak_ptr<ZScene> scene;
	ZHGameObject parent;
	ZComponentList components;
	ZGameObjectList children;
	ZGameObjectProperties properties;

	std::shared_ptr<ZRenderStateGroup> renderState;
	std::shared_ptr<ZUniformBuffer> uniformBuffer;

	struct
	{
		std::mutex position;
		std::mutex orientation;
		std::mutex scale;
		std::mutex modelMatrix;
		std::mutex localModelMatrix;
	} objectMutexes;

    virtual void OnCreate() { }
    virtual void OnDeserialize(const std::shared_ptr<ZOFObjectNode>& dataNode) { }

private:

	static ZIDSequence idGenerator_;

};

class ZGameObjectManager : public ZResourceManager<ZGameObject, ZHGameObject>
{

public:

    virtual ~ZGameObjectManager() = default;

public:

    virtual void Initialize() override { }
    virtual void CleanUp() override { }

public:

    void Initialize(const ZHGameObject& handle);
    void Prepare(const ZHGameObject& handle, double deltaTime);
    void PrepareChildren(const ZHGameObject& handle, double deltaTime);

    void CalculateDerivedData(const ZHGameObject& handle);
    ZHGameObject Clone(const ZHGameObject& handle);
    void AddChild(const ZHGameObject& handle, const ZHGameObject& child);
    void RemoveChild(const ZHGameObject& handle, const ZHGameObject& child, bool recurse = false);
    bool HasChildren(const ZHGameObject& handle);
    bool IsVisible(const ZHGameObject& handle);
    void Destroy(const ZHGameObject& handle);

    bool Active(const ZHGameObject& handle) const;
    std::shared_ptr<ZScene> Scene(const ZHGameObject& handle);
    ZHGameObject Parent(const ZHGameObject& handle);
    std::string Name(const ZHGameObject& handle);
    ZRenderLayer RenderLayer(const ZHGameObject& handle);
    const ZComponentList& Components(const ZHGameObject& handle);
    const ZGameObjectList& Children(const ZHGameObject& handle);
    glm::vec3 Position(const ZHGameObject& handle);
    glm::vec3 Scale(const ZHGameObject& handle);
    glm::quat Orientation(const ZHGameObject& handle);
    glm::vec3 Front(const ZHGameObject& handle);
    glm::vec3 Up(const ZHGameObject& handle);
    glm::vec3 Right(const ZHGameObject& handle);
    glm::mat4 ModelMatrix(const ZHGameObject& handle);
    glm::vec3 PreviousPosition(const ZHGameObject& handle);
    glm::vec3 PreviousFront(const ZHGameObject& handle);
    glm::vec3 PreviousUp(const ZHGameObject& handle);
    glm::vec3 PreviousRight(const ZHGameObject& handle);
    std::shared_ptr<ZRenderStateGroup> RenderState(const ZHGameObject& handle);

    void SetParent(const ZHGameObject& handle, const ZHGameObject& parent);
    void SetScene(const ZHGameObject& handle, const std::shared_ptr<ZScene>& scene);
    void SetPosition(const ZHGameObject& handle, const glm::vec3& position);
    void SetScale(const ZHGameObject& handle, const glm::vec3& scale);
    void SetOrientation(const ZHGameObject& handle, const glm::quat& quaternion);
    void SetOrientation(const ZHGameObject& handle, const glm::vec3& euler);
    void SetLocalModelMatrix(const ZHGameObject& handle, const glm::mat4& modelMatrix);
    void SetModelMatrix(const ZHGameObject& handle, const glm::mat4& modelMatrix);
    void SetRenderOrder(const ZHGameObject& handle, ZRenderLayer renderOrder);
    void SetName(const ZHGameObject& handle, const std::string& name);
    void SetActive(const ZHGameObject& handle, bool active = true);

    void Translate(const ZHGameObject& handle, const glm::vec3& translation, bool global = false);

    template<class T>
    typename std::enable_if<std::is_base_of<ZComponent, T>::value>::type
        AddComponent(const ZHGameObject& handle, std::shared_ptr<T> component)
    {
        std::shared_ptr<T> foundComponent = FindComponent<T>();
        if (foundComponent == nullptr || is_multiple_components_supported<T>::value)
        {
            component->object_ = this;
            components_.push_back(component);
        }
    }

    template<class T>
    std::shared_ptr<T> RemoveComponent(const ZHGameObject& handle, const std::string& id = "")
    {
        std::shared_ptr<T> removed = nullptr;
        if (removed = FindComponent<T>(id)) {
            components_.erase(found);
        }
        return removed;
    }

    template<class T>
    std::shared_ptr<T> FindComponent(const ZHGameObject& handle, const std::string& id = "")
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

    template<class T = ZGameObject>
    ZHGameObject Child(const ZHGameObject& handle, const std::string& name)
    {
        if (!std::is_base_of<ZGameObject, T>::value) return nullptr;

		assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
		ZGameObject* gameObject = resourcePool_.Get(handle);

        for (auto it = gameObject->children.begin(); it != gameObject->children.end(); it++)
        {
            T* childGameObject = resourcePool_.Get<T>(*it);
            if (childGameObject != nullptr && zenith::strings::HasSuffix(childGameObject->name, name))
            {
                return *it;
            }
        }

        return ZHGameObject();
    }

    DECLARE_OBJECT_CREATORS(ZGameObject)
};
