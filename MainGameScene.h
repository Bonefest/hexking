#ifndef MAINGAMESCENE_H_INCLUDED
#define MAINGAMESCENE_H_INCLUDED

#include "cocos2d.h"
#include <vector>

float radians(float degrees) {
    return M_PI / 180.0f * degrees;
}

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
        m_ptempHexDrawer = cocos2d::DrawNode::create();
        addChild(m_ptempHexDrawer);

        scheduleUpdate();
        return true;
    }


    void update(float delta) {
        m_ptempHexDrawer->clear();

        for(int x = -2; x <= 2; x++) {
            for(int y = -2;y <= 2; y++) {
                auto hex = hexagon(cubeToRect(cocos2d::Vec2(x + 3, y + 1), 24.0f), 24.0f);
                //m_ptempHexDrawer->drawSolidPoly(hex.data(), 6, cocos2d::Color4F::YELLOW);
                m_ptempHexDrawer->drawPoly(hex.data(), 6, true, cocos2d::Color4F::WHITE);
            }
        }
    }

private:
    cocos2d::DrawNode* m_ptempHexDrawer;
};

#endif // MAINGAMESCENE_H_INCLUDED
