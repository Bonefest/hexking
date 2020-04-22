#ifndef COMPONENTS_H_INCLUDED
#define COMPONENTS_H_INCLUDED

#include "cocos2d.h"
#include <vector>

namespace hk {

    enum class HexagonStatus { ACTIVE, UNACTIVE };

    struct HexagonView {
        explicit HexagonView(cocos2d::Vec2 t_hexCoordinate,
                             cocos2d::Color4F t_fillColor,
                             cocos2d::Color4F t_borderColor,
                             HexagonStatus t_status = HexagonStatus::ACTIVE): coordinate(t_hexCoordinate),
                                                                              fillColor(t_fillColor),
                                                                              borderColor(t_borderColor),
                                                                              status(t_status) { }

        cocos2d::Vec2 coordinate;

        cocos2d::Color4F fillColor;
        cocos2d::Color4F borderColor;

        HexagonStatus status;
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
