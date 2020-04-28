#include "GameMap.h"
#include "helper.h"

namespace hk {
    GameMap::GameMap(map_size t_size): m_size(t_size) {
        for(uint32_t i = 0; i < t_size.first * t_size.second; i++) {
            m_gameMap.push_back(entt::null);
        }
    }

    void GameMap::setHexagon(cocos2d::Vec2 hex, entt::entity hexagon) {
        if(isOutOfBounds(hex)) return;

        m_gameMap[getIndex(hex)] = hexagon;
    }

    entt::entity GameMap::getHexagon(cocos2d::Vec2 hex) const {
        if(isOutOfBounds(hex)) return entt::null;

        return m_gameMap[getIndex(hex)];
    }

    entt::entity GameMap::getHexagonAtPixel(cocos2d::Vec2 pixel, float hexSize) const {
        return getHexagon(rectToHexCoords(pixel, hexSize));
    }

    int GameMap::getIndex(cocos2d::Vec2 hex) const {
        return static_cast<int>(std::round(hex.y) * m_size.second + std::round(hex.x));
    }

    bool GameMap::isOutOfBounds(cocos2d::Vec2 hex) const {
        return (std::round(hex.x) < 0 || std::round(hex.x) >= m_size.first ||
                std::round(hex.y) < 0 || std::round(hex.y) >= m_size.second);
    }

    bool GameMap::hasFriendNeighbour(entt::registry& registry, cocos2d::Vec2 hex, Team friendTeam) const {
        auto neighbours = getNeighbours(registry, hex);

        for(auto neighbour: neighbours) {
            if(auto hexagonComponent = registry.try_get<Hexagon>(neighbour); hexagonComponent) {
                if(hexagonComponent->team == friendTeam) {
                    return true;
                }
            }
        }

        return false;
    }

    bool GameMap::hasEnemyNeighbour(entt::registry& registry, cocos2d::Vec2 hex, Team friendTeam) const {
        auto neighbours = getNeighbours(registry, hex);

        for(auto neighbour: neighbours) {
            if(auto hexagonComponent = registry.try_get<Hexagon>(neighbour); hexagonComponent) {
                if(hexagonComponent->team != friendTeam && hexagonComponent->team != Team::NO_TEAM) {
                    return true;
                }
            }
        }

        return false;
    }


    std::vector<entt::entity> GameMap::getNeighbours(entt::registry& registry, cocos2d::Vec2 hex) const {
        auto cube = hexToCube(hex);

        cocos2d::Vec3 directions[] = {
            cocos2d::Vec3( 0, 1,-1),
            cocos2d::Vec3( 1, 0,-1),
            cocos2d::Vec3( 1,-1, 0),
            cocos2d::Vec3( 0,-1, 1),
            cocos2d::Vec3(-1, 0, 1),
            cocos2d::Vec3(-1, 1, 0)
        };

        std::vector<entt::entity> result;

        for(cocos2d::Vec3 direction : directions) {
            auto hexagon = getHexagon(cubeToHex(cube + direction));
            if(!registry.valid(hexagon)) continue;

            result.push_back(hexagon);
        }

        return result;
    }
}
