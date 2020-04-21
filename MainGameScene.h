#ifndef MAINGAMESCENE_H_INCLUDED
#define MAINGAMESCENE_H_INCLUDED

#include "cocos2d.h"
#include <vector>

float radians(float degrees) {
    cocos2d::log("%f", degrees);
    return M_PI / 180.0f * degrees;
}

std::vector<cocos2d::Vec2> hexagon(cocos2d::Vec3 position, float size) {
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
        m_ptempHexDrawer = cocos2d::DrawNode::create();
        addChild(m_ptempHexDrawer);

        scheduleUpdate();
        return true;
    }


    void update(float delta) {
        m_ptempHexDrawer->clear();

        auto hex = hexagon(cocos2d::Vec2(200.0f, 200.0f), 24.0f);
        m_ptempHexDrawer->drawSolidPoly(hex.data(), 6, cocos2d::Color4F::YELLOW);
    }

private:
    cocos2d::DrawNode* m_ptempHexDrawer;
};

#endif // MAINGAMESCENE_H_INCLUDED
