#include "GameMap.h"

namespace hk {
    GameMap::GameMap(map_size t_size): m_size(t_size) {
        for(uint32_t i = 0; i < t_size.first * t_size.second; i++) {
            m_gameMap.push_back(entt::entity::null);
        }
    }

    entt::entity GameMap::getHexagon(cocos2d::Vec2 hex) {

    }

    bool GameMap::isOutOfBounds(cocos2d::Vec2 hex) const {
        return (hex.x < 0 || hex.x >= m_size.first ||
                hex.y < 0 || hex.y >= m_size.second);
    }
}
