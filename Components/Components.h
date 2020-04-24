#ifndef COMPONENTS_H_INCLUDED
#define COMPONENTS_H_INCLUDED

#include "cocos2d.h"
#include <vector>
#include <map>

#include "../Dependencies/entt.hpp"
#include "../States/StateOwner.h"

namespace hk {

    enum class Role { ATTACKER, DEFENSER, WORKER };
    enum class Team { TEAM_1, TEAM_2, TEAM_3, TEAM_4, NO_TEAM };

    struct Hexagon {
        explicit Hexagon(cocos2d::Vec2 t_position, Team t_team = Team::NO_TEAM): position(t_position),
                                                                                 team(t_team) { }
        cocos2d::Color4F displayColor;
        cocos2d::Vec2 position;

        Team team;
        StateOwner stateOwner;
    };

    struct HexagonRole {
        explicit HexagonRole(Role t_role, int t_level): role(t_role),
                                                        hp(0.0f),
                                                        currentHp(0.0f),
                                                        damage(0.0f),
                                                        income(0.0f),
                                                        incomePeriod(0.0f) {
            //Call stats calculator function or class method

            level = std::min(std::max(1, t_level), 6);

            switch(t_role) {
                case Role::ATTACKER: {
                    currentHp = hp = t_level * 10.0f;
                    damage = t_level * 1.0f;
                    income = 0.0f;
                    break;
                }

                case Role::DEFENSER: {
                    currentHp = hp = t_level * 20.0f;
                    damage = t_level * 0.3f;
                    income = 0.0f;
                    break;
                }

                case Role::WORKER: {
                    currentHp = hp = t_level * 3.0f;
                    damage = t_level * 0.1f;
                    income = t_level * 10.0f;
                    incomePeriod = 1.1f - t_level * 0.1f;
                    break;
                }
            }
        }

        Role role;

        float hp;
        float currentHp;

        float damage;
        float income;
        float incomePeriod;

        int level;
    };

    struct Player {
        explicit Player(Team t_team): team(t_team),
                                      resources(0) { }

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

        int playersSize;
        std::map<Team, entt::entity> players;
        Team controllableTeam;
    };

}

//TODO Events: onHexagonTouched, onHexagonReleased, onHexagonCanceled

#endif // COMPONENTS_H_INCLUDED
