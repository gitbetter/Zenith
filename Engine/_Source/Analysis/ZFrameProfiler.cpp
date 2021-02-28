/*

     ______     ______     __   __     __     ______   __  __
    /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
    \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
        /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
        \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

        ZFrameProfiler.cpp

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

#include "ZCommon.hpp"
#include "ZFrameProfiler.hpp"

std::array<ZFrameProfileSession, ZFrameProfiler::maxProfileSessionsBucketSize> ZFrameProfiler::sessions_;
std::vector<ZFrameProfileSession> ZFrameProfiler::sessionStack_;
std::unordered_map<std::string, unsigned int> ZFrameProfiler::sessionIndices_;
std::atomic_uint ZFrameProfiler::currentBucketIndex_ = 0;

void ZFrameProfiler::StartSession(const std::string& name)
{
    ZFrameProfileSession session;
    session.name = name;
    session.startTime = SECONDS_TIME;
    sessionStack_.push_back(session);
}

void ZFrameProfiler::CollectSessionVertices(int32_t vertexCount)
{
    sessionStack_.back().frameStats.totalVertices += vertexCount;
}

void ZFrameProfiler::CollectSessionDrawCalls(int32_t drawCalls)
{
    sessionStack_.back().frameStats.totalDrawCalls += drawCalls;
}

void ZFrameProfiler::CollectSessionRenderPasses(int32_t renderPasses)
{
    sessionStack_.back().frameStats.totalRenderPasses += renderPasses;
}

void ZFrameProfiler::EndSession()
{
    auto& session = sessionStack_.back();
    session.endTime = SECONDS_TIME;
    session.frameStats.frameTime = session.endTime - session.startTime;

    double collectedFrameTime = session.frameStats.frameTime;
    unsigned int collectedFrames = 1;
    for (auto i = 0; i < maxProfileSessionsBucketSize; i++) {
        ++collectedFrames;
        collectedFrameTime += (sessions_[i].frameStats.frameTime);
    }
    session.frameStats.framesPerSecond = collectedFrames / collectedFrameTime;

    ZFrameProfileSession endedSession(session);
    sessionStack_.pop_back();

    if (++currentBucketIndex_ >= maxProfileSessionsBucketSize)
        currentBucketIndex_ = 0;

    sessions_[currentBucketIndex_] = endedSession;
    sessionIndices_[endedSession.name] = currentBucketIndex_;
}

ZFrameProfileSession ZFrameProfiler::GetSession(const std::string& name)
{
    return sessions_[sessionIndices_[name]];
}

ZFrameStats ZFrameProfiler::GetSessionStats(const std::string& name)
{
    return sessions_[sessionIndices_[name]].frameStats;
}

ZFrameProfileSession ZFrameProfiler::CurrentSession()
{
    return sessionStack_.back();
}
