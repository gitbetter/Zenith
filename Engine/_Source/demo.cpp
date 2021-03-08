#include "ZGame.hpp"
#include "ZServices.hpp"
#include "ZScene.hpp"
#include "ZEventAgent.hpp"
#include "ZSceneReadyEvent.hpp"
#include "ZAssetStore.hpp"
#include "ZCamera.hpp"

void onSceneLoad(const std::shared_ptr<ZSceneReadyEvent>& event);

int main(int argc, const char* argv[]) {
    // Create a new game instance
    std::shared_ptr<ZGame> game = std::make_shared<ZGame>();

    // Initialize the engine before anything else
    ZGameOptions options;
    options.domain.windowSize.x = 800;
    options.domain.windowSize.y = 600;
    options.domain.maximized = false;
    options.graphics.hasPBR = false;
    game->Initialize(options);

    // Ater initializing the engine, we can access the underlying UI subsystem to register fonts
    ZServices::AssetStore()->RegisterFont("/Fonts/earth_orbiter/earthorbiter.ttf");

    // Register delegate methods for specific builtin events
    ZServices::EventAgent()->Subscribe(&onSceneLoad);

    // Load a default scene from the engine
    ZScene::LoadIn<ZScene>(game, std::initializer_list<std::string>({ "/demo_scene.zof" }));

    // Create the game and start the main game loop. Nothing beyond this point will execute
    // for the duration of the game.
    game->Loop();

    // Make sure to clean up all resources after we're done
    game->CleanUp();

    return 0;
}

void onSceneLoad(const std::shared_ptr<ZSceneReadyEvent>& event)
{
    LOG("Scene '" + event->Scene()->Name() + "' loaded", ZSeverity::Info);
    auto scene = event->Scene();

    if (scene)
    {
        auto camera = scene->ActiveCamera();
        if (camera)
        {
            camera->EnableMovement();
            camera->EnableLook();
        }
        scene->Play();
    }
}