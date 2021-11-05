/*

  ______     ______     __   __     __     ______   __  __
 /\___  \   /\  ___\   /\ "-.\ \   /\ \   /\__  _\ /\ \_\ \
 \/_/  /__  \ \  __\   \ \ \-.  \  \ \ \  \/_/\ \/ \ \  __ \
   /\_____\  \ \_____\  \ \_\\"\_\  \ \_\    \ \_\  \ \_\ \_\
   \/_____/   \/_____/   \/_/ \/_/   \/_/     \/_/   \/_/\/_/

    ZEngine.cpp

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

#include "ZBase.hpp"
#include "ZServices.hpp"
#include "ZAssets.hpp"
#include "ZGLDomain.hpp"
#include "ZGLInput.hpp"
#include "ZBulletPhysicsUniverse.hpp"
#include "ZALAudio.hpp"
#include "ZIDSequence.hpp"
#include "ZOFParser.hpp"

#ifdef DEV_BUILD
#include "ZDevResourceFile.hpp"
#else
#include "ZZipFile.hpp"
#endif

// TODO: Useful to have a config file to parse for more global state info such as window dimensions
// and maximum resource cache size

void ZBase::Initialize(int windowWidth, int windowHeight)
{
    ZGameOptions options;
    options.domain.windowSize.x = windowWidth;
    options.domain.windowSize.y = windowHeight;
    Initialize(options);
}

void ZBase::Initialize(const ZGameOptions& options)
{
    ZServices::Initialize();

    gameOptions_ = options;

    /* ========= Application Window ============ */
    Provide(std::make_shared<ZGLDomain>(options.domain));
    /* =================================== */

    /* ========= Physics System ============ */
    Provide(std::make_shared<ZBulletPhysicsUniverse>());
    /* ===================================== */

    /* ========= Audio System ============ */
    Provide(std::make_shared<ZALAudio>());
    /* ===================================== */

    ZAssets::Initialize();

    Setup();
}

void ZBase::Provide(const std::shared_ptr<ZDomain>& domain)
{
    if (gameSystems_.domain) {
        gameSystems_.domain->CleanUp();
    }

    gameSystems_.domain = domain;
    gameSystems_.domain->Initialize();
}

void ZBase::Provide(const std::shared_ptr<ZPhysicsUniverse>& physics)
{
    if (gameSystems_.physics) {
        gameSystems_.physics->Abort();
        gameSystems_.physics->CleanUp();
    }
    if (!physics)
        return;

    gameSystems_.physics = physics;
    gameSystems_.physics->Initialize();
    ZServices::ProcessRunner(name_)->AttachProcess(gameSystems_.physics);
}

void ZBase::Provide(const std::shared_ptr<ZAudio>& audio)
{
    if (gameSystems_.audio)
        gameSystems_.audio->CleanUp();

    gameSystems_.audio = audio;
    gameSystems_.audio->Initialize();
}

void ZBase::Configure(const ZGameOptions& options)
{
    gameOptions_ = options;
}

void ZBase::CleanUp()
{
    gameSystems_.physics->CleanUp(); gameSystems_.physics.reset();
    gameSystems_.audio->CleanUp(); gameSystems_.audio.reset();
    gameSystems_.domain->CleanUp(); gameSystems_.domain.reset();
}