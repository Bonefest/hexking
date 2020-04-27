#ifndef COMPONENTS_H_INCLUDED
#define COMPONENTS_H_INCLUDED

#include "cocos2d.h"
#include <vector>
#include <map>

#include "../Dependencies/entt.hpp"
#include "../States/StateOwner.h"

#include "HexagonRole.h"

namespace hk {

    enum class Team { TEAM_1, TEAM_2, TEAM_3, TEAM_4, NO_TEAM };

    struct Hexagon {
        explicit Hexagon(cocos2d::Vec2 t_position, Team t_team = Team::NO_TEAM): position(t_position),
                                                                                 team(t_team),
                                                                                 icon(nullptr) { }
        cocos2d::Color4F displayColor;
        cocos2d::Vec2 position;

        Team team;
        StateOwner stateOwner;
        cocos2d::Sprite* icon;
    };

    struct Player {
        explicit Player(Team t_team): team(t_team),
                                      resources(0) { }

        Team team;
        int resources;
        std::vector<cocos2d::Vec2> capturedCells;
    };

    struct PressedHexagon {
        explicit PressedHexagon(double t_pressingTime): pressingTime(t_pressingTime) { }
        double pressingTime;
    };

    struct FocusedHexagon {
        int a;
        //pressingTime;
    };

    struct GameData {
        float hexagonSize;
        float lineWidth;
        float releasedTime;

        int playersSize;
        std::map<Team, entt::entity> players;

        Team controllableTeam;
        entt::entity controllablePlayer;
    };

}

//TODO Events: onHexagonTouched, onHexagonReleased, onHexagonCanceled

#endif // COMPONENTS_H_INCLUDED
