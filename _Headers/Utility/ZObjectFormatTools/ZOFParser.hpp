//
//  ZOFParser.hpp
//  Zenith
//
//  Created by Adrian Sanchez on 17/02/2019.
//  Copyright © 2019 Adrian Sanchez. All rights reserved.
//

#pragma once

// Includes
#include <string>

// Forward Declarations
class ZOFTree;

// Class and Data Structure Definitions
class ZOFParser {
private:
public:
    ZOFParser() { }
    ~ZOFParser() { }

    ZOFTree* Parse(std::string zofFile);

protected:

};
