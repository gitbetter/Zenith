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
#include <map>

// Forward Declarations
struct ZOFNode;
struct ZOFObjectNode;
struct ZOFPropertyNode;
struct ZOFAbstractTerminal;
template<typename T> struct ZOFValueTerminal;

// Class and Data Structure Definitions
typedef std::map<std::string, ZOFNode*> ZOFChildMap;
typedef std::vector<ZOFPropertyNode*> ZOFPropertyList;
typedef std::vector<ZOFAbstractTerminal*> ZOFAbstractTerminalList;

struct ZOFNode {
  std::string id;
  ZOFNode* root = nullptr;
  ZOFChildMap children;

  virtual ~ZOFNode() { }

  void Clear() {
    ZOFChildMap::iterator it = children.begin();
    for (; it != children.end(); it++) delete it->second;
    children.clear();
  }

  virtual std::string ToString() {
    std::string objString = (root == nullptr) ? "\n" : "";
    ZOFChildMap::iterator it = children.begin();
    for (; it != children.end(); it++) objString += it->second->ToString();
    return objString;
  }
};

struct ZOFAbstractTerminal {
  ZOFNode* root = nullptr;
  virtual ~ZOFAbstractTerminal() { };
  virtual std::string ToString() { return ""; }
};

template<class T>
struct ZOFValueTerminal : public ZOFAbstractTerminal {
  T value;
  ~ZOFValueTerminal() { }

  std::string ToString() override { return ""; }
};

struct ZOFPropertyNode : public ZOFNode {
  ZOFAbstractTerminalList values;

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
  ZOFPropertyList properties;

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

typedef ZOFValueTerminal<float> ZOFNumber;
typedef ZOFValueTerminal<std::string> ZOFString;
typedef ZOFValueTerminal<std::vector<float>> ZOFNumberList;
typedef ZOFValueTerminal<std::vector<std::string>> ZOFStringList;
typedef ZOFNode ZOFTree;

// Template specializations
template<> std::string ZOFNumber::ToString();
template<> std::string ZOFString::ToString();
template<> std::string ZOFNumberList::ToString();
template<> std::string ZOFStringList::ToString();