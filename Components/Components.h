#ifndef COMPONENTS_H_INCLUDED
#define COMPONENTS_H_INCLUDED

#include "cocos2d.h"
#include <vector>

#include "../Dependencies/entt.hpp"

namespace hk {

    enum class Role { ATTACKER, DEFENSER, WORKER };
    enum class Team { TEAM_1, TEAM_2, TEAM_3, TEAM_4, NO_TEAM };

    struct Hexagon {
        explicit Hexagon(cocos2d::Vec2 t_position, Team t_team = Team::NO_TEAM): position(t_position),
                                                                                 team(t_team) { }
        cocos2d::Color4F displayColor;
        cocos2d::Vec2 position;

        Team team;
    };

    struct HexagonData {
        Role role;

        float hp;
        float currentHp;

        float damage;
        float income;
    };

    struct Player {
        Team team;
        int resources;
        std::vector<cocos2d::Vec2> capturedCells;
    };

    struct Controllable {

    };

    struct GameData {
        float hexagonSize;
        float lineWidth;
        float releasedTime;
    };

}

//TODO Events: onHexagonTouched, onHexagonReleased, onHexagonCanceled

#endif // COMPONENTS_H_INCLUDED
