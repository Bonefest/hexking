#ifndef GAMEMAP_H_INCLUDED
#define GAMEMAP_H_INCLUDED

#include "Dependencies/entt.hpp"

namespace hk {

    typedef std::pair<int, int> map_size;

    class GameMap {
    public:
        GameMap();

        entt::entity getHexagon(cocos2d::Vec2 hex);
        entt::entity getHexagonAtPixel(cocos2d::Vec2 pixel);

        void setHexagon(cocos2d::Vec2 hex, entt::entity hexagon);

        map_size getSize() const;

        bool hasHexagon(cocos2d::Vec2 hex) const;
        bool hasHexagonAtPixel(cocos2d::Vec2 pixel) const;

    private:
        bool isOutOfBounds(cocos2d::Vec2 hex) const;

        std::vector<entt::entity> m_gameMap;
        map_size m_size;
    };
}

#endif // GAMEMAP_H_INCLUDED
