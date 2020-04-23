#ifndef MAINGAMESCENE_H_INCLUDED
#define MAINGAMESCENE_H_INCLUDED

#include "cocos2d.h"
#include <vector>

#include "Systems/System.h"
#include "Systems/SystemsManager.h"

#include "Components/Components.h"

#include "HexagonNode.h"

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

        hk::HexagonNode* hexagon = hk::HexagonNode::createHexagon(24.0f);
        hexagon->setPosition(cocos2d::Vec2(200.0f, 200.0f));
        addChild(hexagon);
        runAction(cocos2d::CallFunc::create(CC_CALLBACK_0(MainGameScene::postInit, this)));

        scheduleUpdate();
        return true;
    }

    void postInit() {
        initContext();
        initSystems();
//
//        m_manager.getRegistry().on_destory<HexagonView>().connect<&MainGameScene::onDestoryHexagonView>(*this);
    }

    void initContext() {
        hk::GameData gameData;

        gameData.hexagonSize = 24.0f;
        gameData.lineWidth = 2.0f;
        gameData.releasedTime = 0.0f;

        m_manager.getRegistry().set<hk::GameData>(gameData);
    }

    void initSystems() {
        m_manager.addSystem(std::make_shared<hk::HexagonRenderingSystem>(), 1);
        m_manager.addSystem(std::make_shared<hk::InputHandlingSystem>(), 2);
    }
//
//    void onDestoryHexagonView(entt::registry& registry, entt::entity hexagon) {
//        auto hexagonViewComponent = registry.get<hk::HexagonView>(hexagon);
//        hexagonViewComponent.node->removeFromParentAndCleanup(true);
//    }

    void update(float delta) {
        m_manager.updateSystems(delta);
    }
private:
    cocos2d::DrawNode* m_ptempHexDrawer;
    hk::SystemsManager m_manager;
};

#endif // MAINGAMESCENE_H_INCLUDED
