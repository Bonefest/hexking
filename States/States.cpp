#include "States.h"
#include "../Components/Components.h"

#include "../helper.h"

namespace hk {

    void HexagonIdle::onBegin(entt::registry& registry, entt::entity hexagon) {
        if(auto* hexagonComponent = registry.try_get<Hexagon>(hexagon); hexagonComponent) {
            if(auto* role = registry.try_get<HexagonRole>(hexagon); role) {
                if(role->role == Role::WORKER) {
                    hexagonComponent->stateOwner.setState(registry, hexagon, std::make_shared<HexagonWorkerIdle>());
                }
            }
        }
    }
    void HexagonIdle::draw(cocos2d::DrawNode* renderer, entt::registry& registry, entt::entity hexagon) {
        GameData& gameData = registry.ctx<GameData>();
        Hexagon& hexagonComponent = registry.get<Hexagon>(hexagon);

        auto vertices = generateHexagonVertices(gameData.hexagonSize,
                                                        hexToRectCoords(hexagonComponent.position, gameData.hexagonSize));
        cocos2d::Color4F fillColor(0.0f, 0.0f, 0.0f, 0.0f);
        cocos2d::Color4F borderColor = cocos2d::Color4F::WHITE;
        if(hexagonComponent.team != Team::NO_TEAM) {
            fillColor = getTeamColor(hexagonComponent.team);
            borderColor = fillColor * 0.5f;
        }

        //if has role -> draw one more polygon with size equal to fraction hp/maxHp * hexagonSize

        renderer->drawPolygon(vertices.data(),
                                            6,
                                            fillColor,
                                            2.0f,
                                            borderColor);
    }

}
