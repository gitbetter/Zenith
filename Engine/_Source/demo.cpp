#include "ZGame.hpp"
#include "ZScene.hpp"
#include "ZUI.hpp"
#include "ZEventAgent.hpp"
#include "ZSceneReadyEvent.hpp"
#include "ZDomain.hpp"

void onSceneLoad(const std::shared_ptr<ZEvent>& event);

int main(int argc, const char* argv[]) {
    // Create a new game instance
    std::shared_ptr<ZGame> game = std::make_shared<ZGame>();

    // Initialize the engine before anything else
    ZDomainOptions windowOptions;
    windowOptions.width = 800;
    windowOptions.height = 600;
    windowOptions.maximized = false;
    zenith::Initialize(game, windowOptions);

    // Ater initializing the engine, we can access the underlying UI subsystem to register fonts
    zenith::UI()->RegisterFont("/Fonts/earth_orbiter/earthorbiter.ttf");

    // Load a default scene from the engine
    zenith::LoadScene<ZScene>(std::initializer_list<std::string>({ "/demo_scene.zof" }));

    // Register delegate methods for specific UI events
    ZEventDelegate sceneReadyDelegate(&onSceneLoad);
    zenith::EventAgent()->AddListener(sceneReadyDelegate, ZSceneReadyEvent::Type);

    // Free up window cursor. It is captured by default.
    zenith::Domain()->Strategy()->ReleaseCursor();

    // Create the game and start the main game loop. Nothing beyond this point will execute
    // for the duration of the game.
    game->RunGameLoop();

    // Make sure to clean up all resources after we're done
    zenith::CleanUp();

    return 0;
}

void onSceneLoad(const std::shared_ptr<ZEvent>& event)
{
    std::shared_ptr<ZSceneReadyEvent> sceneReadyEvent = std::dynamic_pointer_cast<ZSceneReadyEvent>(event);
    zenith::Log("Scene '" + sceneReadyEvent->Scene()->Name() + "' loaded", ZSeverity::Info);
    sceneReadyEvent->Scene()->Play();
}