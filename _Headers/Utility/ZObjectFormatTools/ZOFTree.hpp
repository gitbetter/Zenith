//
//  ZOFTree.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 17/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include <string>
#include <vector>

// Forward Declarations
struct ZOFObjectNode;

// Class and Data Structure Definitions
struct ZOFNode {
  std::string id;
  ZOFNode* root;
  std::vector<ZOFNode*> children;

  virtual ~ZOFNode() { }

  void Clear() {
    for (ZOFNode* child : children) delete child;
    children.clear();
  }

  virtual std::string ToString() {
    std::string objString;
    for (ZOFNode* child : children) objString += child->ToString();
    return objString;
  }
};

struct ZOFAbstractTerminal {
  ZOFNode* root;
  virtual ~ZOFAbstractTerminal() { };
  virtual std::string ToString() { return ""; }
};

template<typename T>
struct ZOFValueTerminal : public ZOFAbstractTerminal {
  T value;
  ~ZOFValueTerminal() { }

  std::string ToString() override {
    return " [value]";
  }
};

struct ZOFPropertyNode : public ZOFNode {
  std::vector<ZOFAbstractTerminal*> values;

  ~ZOFPropertyNode() {
    for (ZOFAbstractTerminal* val : values) delete val;
  }

  std::string ToString() override {
    std::string propString = "\t:" + id;
    for (ZOFAbstractTerminal* val : values) propString += val->ToString();
    propString += ZOFNode::ToString();
    propString += ":\n";
    return propString;
  }
};

struct ZOFObjectNode : public ZOFNode {
  std::vector<ZOFPropertyNode*> properties;

  ~ZOFObjectNode() {
    for (ZOFPropertyNode* prop : properties) delete prop;
  }

  std::string ToString() override {
    std::string objString = id + "\n";
    for (ZOFPropertyNode* prop : properties) objString += prop->ToString();
    objString += ZOFNode::ToString();
    objString += "-\n";
    return objString;
  }
};

typedef ZOFNode ZOFTree;
