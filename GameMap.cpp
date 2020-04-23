#include "GameMap.h"
#include "helper.h"

namespace hk {
    GameMap::GameMap(map_size t_size): m_size(t_size) {
        for(uint32_t i = 0; i < t_size.first * t_size.second; i++) {
            m_gameMap.push_back(entt::null);
        }
    }

    entt::entity GameMap::getHexagon(cocos2d::Vec2 hex) {
        if(isOutOfBounds(hex)) return entt::null;

        return m_gameMap[getIndex(hex)];
    }

    entt::entity GameMap::getHexagonAtPixel(cocos2d::Vec2 pixel, float hexSize) {
        return getHexagon(rectToHexCoords(pixel, hexSize));
    }

    int GameMap::getIndex(cocos2d::Vec2 hex) const {
        return static_cast<int>(std::round(hex.y) * m_size.second + std::round(hex.x));
    }

    bool GameMap::isOutOfBounds(cocos2d::Vec2 hex) const {
        return (std::round(hex.x) < 0 || std::round(hex.x) >= m_size.first ||
                std::round(hex.y) < 0 || std::round(hex.y) >= m_size.second);
    }
}
