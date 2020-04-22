#ifndef MAINGAMESCENE_H_INCLUDED
#define MAINGAMESCENE_H_INCLUDED

#include "cocos2d.h"
#include <vector>

#include "Systems/System.h"
#include "Systems/SystemsManager.h"

cocos2d::Vec2 cubeToRect(cocos2d::Vec2 position, float size) {
    cocos2d::Vec2 result(cocos2d::Vec2::ZERO);
    result.x += std::round(position.x) * 1.5f * size;
    result.y += std::round(position.x) * 0.5f * std::sqrt(3) * size;

    result.y += std::round(position.y) * std::sqrt(3) * size;

    return result;
}

std::vector<cocos2d::Vec2> hexagon(cocos2d::Vec2 position, float size) {
    std::vector<cocos2d::Vec2> vertices;
    for(int i = 0;i < 6; ++i) {
        vertices.push_back(cocos2d::Vec2(std::cos(radians(i * 60.0f)),
                                         std::sin(radians(i * 60.0f))) * size + position);
    }
    return vertices;
}

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

        m_manager.addSystem(std::make_shared<hk::HexagonRenderingSystem>());

        m_ptempHexDrawer = cocos2d::DrawNode::create();
        addChild(m_ptempHexDrawer);

        scheduleUpdate();
        return true;
    }


    void update(float delta) {
        m_manager.updateSystems(delta);

        m_ptempHexDrawer->clear();
        auto hex = hexagon(cubeToRect(cocos2d::Vec2(0.0f, 0.0f), 24.0f), 24.0f);
        m_ptempHexDrawer->drawPolygon(hex.data(), 6, cocos2d::Color4F(1.0f, 1.0f, 1.0f, 0.1f), 2.0f, cocos2d::Color4F::WHITE);
    }
private:
    cocos2d::DrawNode* m_ptempHexDrawer;
    hk::SystemsManager m_manager;
};

#endif // MAINGAMESCENE_H_INCLUDED
