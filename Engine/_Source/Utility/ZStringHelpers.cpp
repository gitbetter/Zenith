/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZStringHelpers.cpp

    Created by Adrian Sanchez on 15/09/2020.
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

#include "ZStringHelpers.hpp"

namespace zenith
{
    namespace strings
    {
        std::string FormatStringGlobals(const std::string& str)
        {
            std::string s(str);
            std::smatch matches;
            if (std::regex_match(s, matches, std::regex(".*\\$(.*)\\$.*")))
            {
                for (unsigned int i = 1; i < matches.size(); i++)
                {
                    if (matches.str(i) == "ENGINE_ROOT")
                    {
                        s = std::regex_replace(s, std::regex("\\$ENGINE_ROOT\\$"), ENGINE_ROOT);
                    }
                    else if (matches.str(i) == "EDITOR_ROOT")
                    {
                    #ifdef EDITOR_ROOT
                        s = std::regex_replace(s, std::regex("\\$EDITOR_ROOT\\$"), EDITOR_ROOT);
                    #endif
                    }
                }
            }
            return s;
        }

        bool HasGameObjectPrefix(const std::string& str)
        {
            static std::unordered_map<std::string, std::string> goPrefixes{
                {"ZGO", "ZGO"},
                {"ZTR", "ZTR"},
                {"ZLT", "ZLT"},
                {"ZSKY", "ZSKY"},
                {"ZGR", "ZGR"}
            };
            return goPrefixes.find(str.substr(0, str.find("_"))) != goPrefixes.end();
        }

        bool HasUIPrefix(const std::string& str)
        {
            static std::unordered_map<std::string, std::string> uiPrefixes{
                {"ZUI", "ZUI"}
            };
            return uiPrefixes.find(str.substr(0, str.find("_"))) != uiPrefixes.end();
        }

        bool HasSuffix(const std::string& str, const std::string& suffix)
        {
            return suffix.size() <= str.size() && std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
        }
    }
}