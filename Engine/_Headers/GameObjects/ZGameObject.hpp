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

#include "ZResourceManager.hpp"
#include "ZOFTree.hpp"

class ZScene;
class ZUniformBuffer;
class ZRenderStateGroup;

enum class ZGameObjectType
{
    Custom, Camera, Grass, Light, Particle, SceneRoot, Skybox
};

struct ZGameObjectProperties
{
    ZRenderLayer renderOrder{ ZRenderLayer::Static };
    glm::vec3 position, previousPosition;
    glm::vec3 scale, previousScale;
    glm::quat orientation, previousOrientation;
    glm::mat4 localModelMatrix, modelMatrix;
    std::string name;
    bool active = true;
    bool visible = true;
};

struct ZGameObject
{
    
    ZGameObject();

    ZHGameObject handle;
    ZGameObjectType type;
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
    virtual void OnCloned(ZGameObject* original) { }
    virtual void OnUpdate(double deltaTime) { }
    virtual void OnUpdateChildren(double deltaTime) { }

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

    ZHGameObject Create(const ZGameObjectType& type, const ZHGameObject& restoreHandle = ZHGameObject());
    ZHGameObject CreateReady(const ZGameObjectType& type, const std::shared_ptr<ZScene>& scene = nullptr, const ZHGameObject& restoreHandle = ZHGameObject());
    ZHGameObject Deserialize(const ZOFHandle& dataHandle, const std::shared_ptr<ZOFObjectNode>& dataNode, const std::shared_ptr<ZScene>& scene = nullptr);

    void Update(const ZHGameObject& handle, double deltaTime);
    void UpdateChildren(const ZHGameObject& handle, double deltaTime);

    void CalculateDerivedData(const ZHGameObject& handle);
    ZHGameObject Clone(const ZHGameObject& handle);
    void AddChild(const ZHGameObject& handle, const ZHGameObject& child);
    void RemoveChild(const ZHGameObject& handle, const ZHGameObject& child, bool recurse = false);
    bool HasChildren(const ZHGameObject& handle);
    bool IsVisible(const ZHGameObject& handle);
    void Destroy(const ZHGameObject& handle);

    bool Active(const ZHGameObject& handle);
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

    void AddComponent(const ZHGameObject& handle, const ZHComponent& component);
    ZHComponent RemoveComponent(const ZHGameObject& handle, const std::string& name = "");
	ZHComponent RemoveComponent(const ZHGameObject& handle, const ZHComponent& component);

    template<class T = ZComponent>
    ZHComponent FindComponent(const ZHGameObject& handle, const std::string& name = "")
    {
		assert(!handle.IsNull() && "Cannot fetch property with a null game object handle!");
		ZGameObject* gameObject = resourcePool_.Get(handle);

        ZComponentList::iterator found;

        if (!name.empty())
        {
            found = std::find_if(gameObject->components.begin(), gameObject->components.end(), [&name](const auto& comp) {
                ZComponent* compObj = ZServices::ComponentManager()->Dereference(comp);    
                return compObj != nullptr && name == compObj->name;
            });
        }
        else
        {
            found = std::find_if(gameObject->components.begin(), gameObject->components.end(), [&name](const auto& comp) {
                return ZServices::ComponentManager()->Dereference<T>(comp) != nullptr;
            });
        }

        if (found != gameObject->components.end())
        {
            return *found;
        }

        return ZHComponent();
    }

    template<class T = ZGameObject>
    ZHGameObject Child(const ZHGameObject& handle, const std::string& name)
    {
        if (!std::is_base_of<ZGameObject, T>::value)
        {
            return ZHGameObject();
        }

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
};
