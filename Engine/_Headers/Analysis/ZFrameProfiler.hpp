/*

     ______     ______     __   __     __     ______   __  __
    /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
    \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
      /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
      \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

        ZFrameProfiler.hpp

        Created by Adrian Sanchez on 02/26/2021.
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

struct ZFrameStats
{
    uint32_t totalDrawCalls = 0;
    uint32_t totalVertices = 0;
    uint16_t totalRenderPasses = 0;
    uint16_t framesPerSecond = 0;
    double frameTime = 0;
};

struct ZFrameProfileSession
{
    std::string name;
    ZFrameStats frameStats;
    double startTime = 0;
    double endTime = 0;
};

class ZFrameProfiler {

    static constexpr unsigned int maxProfileSessionsBucketSize = 7;

public:

    static ZFrameProfileSession CurrentSession();

    static ZFrameProfileSession GetSession(const std::string& name);

    static ZFrameStats GetSessionStats(const std::string& name);

    static void StartSession(const std::string& name);

    static void CollectSessionVertices(int32_t vertexCount);

    static void CollectSessionDrawCalls(int32_t drawCalls);

    static void CollectSessionRenderPasses(int32_t renderPasses);

    static void EndSession();

private:

    static std::array<ZFrameProfileSession, maxProfileSessionsBucketSize> sessions_;
    static std::vector<ZFrameProfileSession> sessionStack_;
    static std::unordered_map<std::string, unsigned int> sessionIndices_;
    static std::atomic_uint currentBucketIndex_;

};