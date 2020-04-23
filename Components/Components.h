#ifndef COMPONENTS_H_INCLUDED
#define COMPONENTS_H_INCLUDED

#include "cocos2d.h"
#include <vector>

#include "HexagonNode.h"

namespace hk {

    enum class HexagonStatus { ACTIVE, UNACTIVE };

    struct HexagonView {
        explicit HexagonView(cocos2d::Vec2 t_hexCoordinate,
                             cocos2d::Color3B t_color,
                             float t_hexSize): node(nullptr) {
            auto runningScene = cocos2d::Director::getInstance()->getRunningScene();
            node = HexagonNode::createHexagon(t_hexSize);
            node->setColor(t_color);
            node->setPosition(hexToRectCoords(t_hexCoordinate, t_hexSize));
            runningScene->addChild(node);

        }

        HexagonNode* node;
    };

    enum class HexagonType { ATTACKER, DEFENSER, WORKER };

    struct HexagonData {
        HexagonType type;
        int level;
        float damage;
        float defense;
        float income;

        float currentHp;

        bool fighting;
    };

    struct PlayerData {
        cocos2d::Color4F color;
        int resources;
        std::vector<cocos2d::Vec2> capturedCells;
    };

    struct GameData {
        float hexagonSize;
        float lineWidth;
        float releasedTime;
    };

}

//TODO Events: onHexagonTouched, onHexagonReleased, onHexagonCanceled

#endif // COMPONENTS_H_INCLUDED
