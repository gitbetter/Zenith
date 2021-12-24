#include "ZGame.hpp"
#include "ZServices.hpp"
#include "ZScene.hpp"
#include "ZEventAgent.hpp"
#include "ZSceneReadyEvent.hpp"
#include "ZAssets.hpp"
#include "ZCamera.hpp"
#include "ZGraphicsComponent.hpp"

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

    // After initializing the engine, we can access the underlying UI subsystem to register fonts
    ZAssets::FontManager()->CreateAsync("/Fonts/earth_orbiter/earthorbiter.ttf", 64);

    // Register delegate methods for specific built in events
    ZServices::EventAgent()->Subscribe(&onSceneLoad);

    // Load a default scene from the engine
    auto scene = ZScene::LoadIn<ZScene>(game);
	scene->Initialize();
	scene->SetDefaultRenderPasses();

	ZHGameObject camera = ZAssets::GameObjectManager()->CreateReady(ZGameObjectType::Camera, scene);
	ZAssets::GameObjectManager()->SetName(camera, "Camera");
	ZAssets::GameObjectManager()->SetPosition(camera, glm::vec3(0.0f, 10.0f, -10.0f));
	ZAssets::GameObjectManager()->SetOrientation(camera, glm::vec3(45.0f, 0.0f, 0.0f));
	ZAssets::GameObjectManager()->Dereference<ZCamera>(camera)->movementStyle = ZCameraMovementStyle::Follow;

	ZHGameObject light = ZAssets::GameObjectManager()->CreateReady(ZGameObjectType::Light, scene);
	ZAssets::GameObjectManager()->SetName(light, "Light");
	ZAssets::GameObjectManager()->SetPosition(light, glm::vec3(0.0f, 50.0f, 0.0f));
	ZAssets::GameObjectManager()->SetOrientation(light, glm::vec3(45.0f, 0.0f, 0.0f));
	ZAssets::GameObjectManager()->Dereference<ZLight>(light)->lightType = ZLightType::Directional;
	ZAssets::GameObjectManager()->Dereference<ZLight>(light)->lightProperties.ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	ZAssets::GameObjectManager()->Dereference<ZLight>(light)->lightProperties.color = glm::vec4(0.95f, 0.95f, 0.95f, 1.0f);

	ZHGameObject plane = ZAssets::GameObjectManager()->CreateReady(ZGameObjectType::Custom, scene);
	ZAssets::GameObjectManager()->SetName(plane, "Plane");
	ZAssets::GameObjectManager()->SetPosition(plane, glm::vec3(0.0f, 0.0f, 0.0f));
	ZAssets::GameObjectManager()->SetScale(plane, glm::vec3(50.0f, 0.1f, 50.0f));

	ZHComponent planeGraphicsComp = ZAssets::ComponentManager()->CreateIn(ZComponentType::Graphics, plane);
	ZHModel planeModel = ZAssets::ModelManager()->Create(ZModelType::Plane);
	ZAssets::ComponentManager()->Dereference<ZGraphicsComponent>(planeGraphicsComp)->Initialize(planeModel);
	ZAssets::ComponentManager()->Dereference<ZGraphicsComponent>(planeGraphicsComp)->AddMaterial(ZAssets::MaterialManager()->Default());

	ZHGameObject cube = ZAssets::GameObjectManager()->CreateReady(ZGameObjectType::Custom, scene);
	ZAssets::GameObjectManager()->SetName(cube, "Cube");
	ZAssets::GameObjectManager()->SetPosition(cube, glm::vec3(0.0f, 10.0f, 0.0f));

	ZHComponent cubeGraphicsComp = ZAssets::ComponentManager()->CreateIn(ZComponentType::Graphics, cube);
	ZHModel cubeModel = ZAssets::ModelManager()->Create(ZModelType::Cube);
	ZAssets::ComponentManager()->Dereference<ZGraphicsComponent>(cubeGraphicsComp)->Initialize(cubeModel);
	ZAssets::ComponentManager()->Dereference<ZGraphicsComponent>(cubeGraphicsComp)->AddMaterial(ZAssets::MaterialManager()->Default());

	scene->AddGameObject(camera);
	scene->AddGameObject(light);
	scene->AddGameObject(plane);
	scene->AddGameObject(cube);

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
        scene->Play();
    }
}