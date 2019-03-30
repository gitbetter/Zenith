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

ZProcessRunner::~ZProcessRunner() {
  AbortAllProcesses(true);
  ClearProcesses();
}

unsigned int ZProcessRunner::UpdateProcesses() {
  unsigned short int successCount = 0, failCount = 0;

  ZProcessList::iterator it = processList_.begin();
  while (it != processList_.end()) {
    std::shared_ptr<ZProcess> process = *it;

    ZProcessList::iterator thisIt = it;
    ++it;

    if (process->State() == ZProcessState::Uninitialized) {
      process->Initialize();
    }

    if (process->State() == ZProcessState::Running) {
      process->Update();
    }

    if (process->IsDead()) {
      switch (process->State()) {
        case ZProcessState::Finished: 
        {
          std::shared_ptr<ZProcess> child = process->RemoveChild();
          if (child) {
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

      processList_.erase(thisIt);
    }
  }

  return ((successCount << 16 | failCount));
}

void ZProcessRunner::ClearProcesses() {
  processList_.clear();
}

void ZProcessRunner::AttachProcess(std::shared_ptr<ZProcess> process) {
  processList_.push_back(process);
}

void ZProcessRunner::AbortAllProcesses(bool immediate) {
  ZProcessList::iterator it = processList_.begin();
  while (it != processList_.end()) {
    ZProcessList::iterator temp = it;
    ++it;

    std::shared_ptr<ZProcess> process = *temp;
    if (process->IsAlive()) {
      if (immediate) process->Abort();
      processList_.erase(temp);
    }
  }
}