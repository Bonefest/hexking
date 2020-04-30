#ifndef MAINGAMESCENE_H_INCLUDED
#define MAINGAMESCENE_H_INCLUDED

#include "ui/UIScrollView.h"
#include "cocos2d.h"
#include <vector>

#include "Systems/System.h"
#include "Systems/SystemsManager.h"

#include "Components/Components.h"

#include "HexagonNode.h"
#include "GameMap.h"

#include "common.h"

#include "ui/UIButton.h"

#include "HexagonRenderer.h"

class MainGameScene: public cocos2d::Scene {
public:
    CREATE_FUNC(MainGameScene);
    static MainGameScene* createScene() {
        return MainGameScene::create();
    }

    bool init() {
        if(!cocos2d::Scene::init()) {
            return false;
        }

        auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

        cocos2d::Camera* worldCamera = cocos2d::Camera::createOrthographic(visibleSize.width, visibleSize.height, 1.0f, 1000.0f);
        worldCamera->initDefault();
        worldCamera->setAnchorPoint(getDefaultCamera()->getAnchorPoint());
        worldCamera->setPosition(getDefaultCamera()->getPosition());
        worldCamera->setCameraFlag(cocos2d::CameraFlag::USER1);
        worldCamera->setTag(hk::Constants::WorldCamera);
        addChild(worldCamera);

        runAction(cocos2d::CallFunc::create(CC_CALLBACK_0(MainGameScene::postInit, this)));

        //TEST


        scheduleUpdate();
        return true;
    }


    void postInit() {
        initContext();
        initSystems();
        generatePlayers(2);
        generateGrid();


    }

    void initContext() {
        hk::GameData gameData;

        gameData.hexagonSize = 24.0f;
        gameData.lineWidth = 2.0f;
        gameData.releasedTime = 0.0f;

        m_manager.getRegistry().set<hk::GameData>(gameData);
        m_manager.getRegistry().set<hk::GameMap>(hk::map_size{32, 32});
    }

    void initSystems() {

        m_manager.addSystem(std::make_shared<hk::HUDRenderingSystem>(), 1);
        m_manager.addSystem(std::make_shared<hk::FloatingTextSystem>(), 1);
        m_manager.addSystem(std::make_shared<hk::HexagonRenderingSystem>(), 1);
        m_manager.addSystem(std::make_shared<hk::HexagonTouchHandleSystem>(), 1);
        m_manager.addSystem(std::make_shared<hk::HexagonTouchActionSystem>(), 1);
        m_manager.addSystem(std::make_shared<hk::HexagonMenuSystem>(), 2);
        m_manager.addSystem(std::make_shared<hk::HexagonUpdatingSystem>(), 3);
        m_manager.addSystem(std::make_shared<hk::WarSystem>(), 3);
        m_manager.addSystem(std::make_shared<hk::InputHandlingSystem>(), 4);
        m_manager.addSystem(std::make_shared<hk::CommandHandlingSystem>(), 4);
    }

    void generatePlayers(int players) {
        auto& registry = m_manager.getRegistry();
        hk::GameData& gameData = registry.ctx<hk::GameData>();

        gameData.playersSize = players;

        for(int i = 0;i < players; i++) {
            hk::Team team = hk::Team(i);

            entt::entity player = registry.create();
            registry.emplace<hk::Player>(player, team);
            gameData.players[team] = player;
        }

        gameData.controllableTeam = hk::Team::TEAM_1;
        gameData.controllablePlayer = gameData.players[gameData.controllableTeam];
    }

    void generateGrid() {
        auto& registry = m_manager.getRegistry();
        hk::GameMap& gameMap = registry.ctx<hk::GameMap>();
        for(int y = 0;y < 16;y++) {
            for(int x = 0;x <16;x++) {
                if(rand() % 2) continue;

                entt::entity hexagon = registry.create();
                hk::Hexagon hexagonComponent(cocos2d::Vec2(x, y));
                hexagonComponent.stateOwner.setState(registry, hexagon, std::make_shared<hk::HexagonIdle>());

                registry.emplace<hk::Hexagon>(hexagon, hexagonComponent);
                gameMap.setHexagon(cocos2d::Vec2(x, y), hexagon);
            }
        }

        //Generating players starting hexagons
        auto isAvailablePosition = [&](cocos2d::Vec2 position)->bool {
            auto entity = gameMap.getHexagon(position);
            return (registry.valid(entity) && !registry.has<hk::HexagonRole>(entity));
        };

        hk::GameData gameData = registry.ctx<hk::GameData>();
        for(int i = 0;i < gameData.playersSize; i++) {
            cocos2d::Vec2 startPosition = cocos2d::Vec2(-32,-32);
            while(!isAvailablePosition(startPosition)) {
                startPosition = cocos2d::Vec2(cocos2d::RandomHelper::random_int(0, 32), cocos2d::RandomHelper::random_int(0, 32));
            }

            auto hexagon = gameMap.getHexagon(startPosition);
            registry.emplace<hk::HexagonRole>(hexagon, hk::Role::WORKER, 1);
            registry.get<hk::Hexagon>(hexagon).team = hk::Team(i);
        }
    }


    void update(float delta) {
        m_manager.updateSystems(delta);
    }
private:
    cocos2d::DrawNode* m_ptempHexDrawer;
    hk::SystemsManager m_manager;
};

#endif // MAINGAMESCENE_H_INCLUDED
