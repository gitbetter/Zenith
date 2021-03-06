/*

   ______     ______     __   __     __     ______   __  __
  /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
  \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
    /\_____\  \ \_____\  \ \_\" \_\  \ \_\    \ \_\  \ \_\ \_\
    \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZProcessRunner.cpp

    Created by Adrian Sanchez on 23/03/2019.
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

#include "ZProcessRunner.hpp"
#include "ZProcess.hpp"

ZProcessRunner::~ZProcessRunner()
{
    AbortAllProcesses(true);
    ClearProcesses();
}

void ZProcessRunner::UpdateTick(double deltaTime)
{
    for (int priority = ZPriority::FirstPriority; priority < ZPriority::LastPriority; priority++)
    {
        UpdateProcesses(deltaTime, (ZPriority) priority);
    }
}

void ZProcessRunner::UpdateProcesses(double deltaTime, ZPriority priority)
{
    unsigned short int successCount = 0, failCount = 0;

    ZProcessList::iterator it = processList_[priority].begin();
    while (it != processList_[priority].end())
    {
        std::shared_ptr<ZProcess> process = *it;

        ZProcessList::iterator thisIt = it;
        ++it;

        if (process->State() == ZProcessState::Uninitialized)
        {
            process->Initialize();
        }

        if (process->State() == ZProcessState::Running)
        {
            process->Update(deltaTime);
        }

        if (process->IsDead())
        {
            switch (process->State())
            {
            case ZProcessState::Finished:
            {
                std::shared_ptr<ZProcess> child = process->RemoveChild();
                if (child)
                {
                    child->SetState(ZProcessState::Uninitialized);
                    if (child) AttachProcess(child);
                }
                else ++successCount;
                break;
            }
            case ZProcessState::Failed:
            case ZProcessState::Aborted:
            {
                ++failCount;
                break;
            }
            default: break;
            }

            processList_[priority].erase(thisIt);
        }
    }
}

void ZProcessRunner::ClearProcesses()
{
    for (int priority = ZPriority::FirstPriority; priority < ZPriority::LastPriority; priority++)
    {
        processList_[priority].clear();
    }
}

void ZProcessRunner::AttachProcess(std::shared_ptr<ZProcess> process, ZPriority priority)
{
    if (std::find(processList_[priority].begin(), processList_[priority].end(), process) != processList_[priority].end())
        return;
    processList_[priority].push_back(process);
}

void ZProcessRunner::AbortAllProcesses(bool immediate)
{
    for (int priority = ZPriority::FirstPriority; priority < ZPriority::LastPriority; priority++)
    {
        ZProcessList::iterator it = processList_[priority].begin();
        while (it != processList_[priority].end())
        {
            ZProcessList::iterator temp = it;
            ++it;

            std::shared_ptr<ZProcess> process = *temp;
            if (process->IsAlive())
            {
                if (immediate) process->Abort();
                processList_[priority].erase(temp);
            }
        }
    }
}