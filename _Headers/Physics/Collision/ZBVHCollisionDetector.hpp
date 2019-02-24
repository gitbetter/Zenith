//
//  ZBVHCollisionDetector.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 20/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include "ZCommon.hpp"

// Forward Declarations
class ZGameObject;

// Class and Data Structure Definitions
struct ZBoundingSphere {
  glm::vec3 center;
  float radius;

  ZBoundingSphere(const glm::vec3 center, float radius);
  ZBoundingSphere(const ZBoundingSphere& one, const ZBoundingSphere& two);

  bool Overlaps(const ZBoundingSphere* other) const;
  float Growth(const ZBoundingSphere& other) const;
  float Size() const {
    return (1.333333f) * glm::pi<float>() * radius * radius * radius;
  }
};

struct ZPotentialContact {
  ZGameObject* body[2];
};

template<class BoundingVolumeType>
class ZBVHNode {
public:

  ZBVHNode* parent;
  ZBVHNode* children[2];
  BoundingVolumeType volume;
  ZGameObject* body;

  ZBVHNode(ZBVHNode* parent, const BoundingVolumeType& boundingVolume, ZGameObject* body = nullptr)
  : parent(parent), volume(boundingVolume), body(body) {
    children[0] = children[1] = nullptr;
  }
  ~ZBVHNode();

  bool IsLeaf() const {
    return body != nullptr;
  }

  unsigned int PotentialContacts(ZPotentialContact* contacts, unsigned int max) const;
  void Insert(ZGameObject* body, const BoundingVolumeType& volume);
  void Remove(ZGameObject* body);

protected:

  bool overlaps(const ZBVHNode<BoundingVolumeType>* other) const;
  unsigned int PotentialContactsWith(const ZBVHNode<BoundingVolumeType>* other, ZPotentialContact* contacts, unsigned int max) const;
  void RecalculateBoundingVolume(bool recurse = true);
};

template<class BoundingVolumeType>
void ZBVHNode<BoundingVolumeType>::Insert(ZGameObject* newBody, const BoundingVolumeType& newVolume) {
  if (IsLeaf()) {
    children[0] = new ZBVHNode<BoundingVolumeType>(this, volume, body);
    children[1] = new ZBVHNode<BoundingVolumeType>(this, newVolume, newBody);
    this->body = nullptr;
    RecalculateBoundingVolume();
  } else {
    if (children[0]->Growth(newVolume) < children[1]->Growth(newVolume)) {
      children[0]->Insert(newBody, newVolume);
    } else {
      children[1]->Insert(newBody, newVolume);
    }
  }
}

template<class BoundingVolumeType>
void ZBVHNode<BoundingVolumeType>::Remove(ZGameObject* toDelete) {
  if (body == toDelete) {
    delete this;
  } else if (children[0]->body == toDelete) {
    delete children[0];
  } else if (children[1]->body == toDelete) {
    delete children[1];
  }
}

template<class BoundingVolumeType>
ZBVHNode<BoundingVolumeType>::~ZBVHNode() {
  if (parent) {
    ZBVHNode<BoundingVolumeType>* sibling;
    if (parent->children[0] == this) sibling = parent->children[1];
    else sibling = parent->children[0];

    parent->volume = sibling->volume;
    parent->body = sibling->body;
    parent->children[0] = sibling->children[0];
    parent->children[1] = sibling->children[1];

    sibling->parent = nullptr;
    sibling->body = nullptr;
    sibling->children[0] = nullptr;
    sibling->children[1] = nullptr;
    delete sibling;

    parent->RecalculateBoundingVolume();
  }

  if (children[0]) {
    children[0]->parent = nullptr;
    delete children[0];
  }

  if (children[1]) {
    children[1]->parent = nullptr;
    delete children[1];
  }
}

template<class BoundingVolumeType>
void ZBVHNode<BoundingVolumeType>::RecalculateBoundingVolume(bool recurse) {
  if (IsLeaf()) return;

  volume = BoundingVolumeType(children[0]->volume, children[1]->volume);

  if (recurse && parent) parent->RecalculateBoundingVolume(true);
}

template<class BoundingVolumeType>
unsigned int ZBVHNode<BoundingVolumeType>::PotentialContacts(ZPotentialContact* contacts, unsigned int max) const {
  if (IsLeaf() || max == 0) return 0;

  return children[0]->PotentialContactsWith(children[1], contacts, max);
}

template<class BoundingVolumeType>
unsigned int ZBVHNode<BoundingVolumeType>::PotentialContactsWith(const ZBVHNode<BoundingVolumeType>* other, ZPotentialContact* contacts, unsigned int max) const {
  if (!overlaps(other) || max == 0) return 0;

  if (IsLeaf() && other->IsLeaf()) {
    contacts->body[0] = body;
    contacts->body[1] = other->body;
    return 1;
  }

  if (other->IsLeaf() || (!IsLeaf() && volume->Size() >= other->volume->Size())) {
    unsigned int count = children[0]->PotentialContactsWith(other, contacts, max);
    if (max > count)
      return count + children[1]->PotentialContactsWith(other, contacts + count, max - count);
  } else {
    unsigned int count = PotentialContactsWith(other->children[0], contacts, max);
    if (max > count)
      return count + PotentialContactsWith(other->children[1], contacts + count, max - count);
  }
}
