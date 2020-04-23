#ifndef MAINGAMESCENE_H_INCLUDED
#define MAINGAMESCENE_H_INCLUDED

#include "cocos2d.h"
#include <vector>

#include "Systems/System.h"
#include "Systems/SystemsManager.h"

#include "Components/Components.h"

#include "HexagonNode.h"
#include "GameMap.h"

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

        runAction(cocos2d::CallFunc::create(CC_CALLBACK_0(MainGameScene::postInit, this)));

        scheduleUpdate();
        return true;
    }


    void postInit() {
        initContext();
        initSystems();
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
        m_manager.addSystem(std::make_shared<hk::HexagonRenderingSystem>(), 1);
        m_manager.addSystem(std::make_shared<hk::InputHandlingSystem>(), 2);
    }

    void generateGrid() {
        auto& registry = m_manager.getRegistry();
        hk::GameMap& gameMap = registry.ctx<hk::GameMap>();
        for(int y = 0;y < 16;y++) {
            for(int x = 0;x <16;x++) {
                if(rand() % 2) continue;

                entt::entity hexagon = registry.create();
                registry.assign<hk::HexagonView>(hexagon, cocos2d::Vec2(x, y), cocos2d::Color4F::WHITE, cocos2d::Color4F::WHITE);
            }
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
