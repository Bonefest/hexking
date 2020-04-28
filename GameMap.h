#ifndef GAMEMAP_H_INCLUDED
#define GAMEMAP_H_INCLUDED

#include "Dependencies/entt.hpp"
#include "cocos2d.h"

#include "Components/Components.h"

namespace hk {

    typedef std::pair<int, int> map_size;

    class GameMap {
    public:
        GameMap(map_size t_size);

        entt::entity getHexagon(cocos2d::Vec2 hex) const;
        entt::entity getHexagonAtPixel(cocos2d::Vec2 pixel, float hexSize) const;

        void setHexagon(cocos2d::Vec2 hex, entt::entity hexagon);

        map_size getSize() const;

        bool hasHexagon(cocos2d::Vec2 hex) const;
        bool hasHexagonAtPixel(cocos2d::Vec2 pixel, float hexSize) const;

        bool hasFriendNeighbour(entt::registry& registry, cocos2d::Vec2 hex, Team friendTeam) const;
        bool hasEnemyNeighbour(entt::registry& registry, cocos2d::Vec2 hex, Team friendTeam) const;

        std::vector<entt::entity> getNeighbours(entt::registry& registry, cocos2d::Vec2 hex) const;

    private:
        int getIndex(cocos2d::Vec2 hex) const;
        bool isOutOfBounds(cocos2d::Vec2 hex) const;

        std::vector<entt::entity> m_gameMap;
        map_size m_size;
    };
}

#endif // GAMEMAP_H_INCLUDED
