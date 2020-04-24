#include "States.h"
#include "../Components/Components.h"

#include "../helper.h"
#include "../common.h"

namespace hk {

    void HexagonIdle::onBegin(entt::registry& registry, entt::entity hexagon) {
        findNextState(registry, hexagon);
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
                              Constants::BORDER_SIZE,
                              borderColor);
    }

    void HexagonIdle::update(entt::registry& registry, entt::dispatcher& dispatcher, entt::entity hexagon, float delta) {
        findNextState(registry, hexagon);
    }

    void HexagonIdle::findNextState(entt::registry& registry, entt::entity hexagon) {
        if(auto* hexagonComponent = registry.try_get<Hexagon>(hexagon); hexagonComponent) {
            if(auto* role = registry.try_get<HexagonRole>(hexagon); role) {
                if(role->role == Role::WORKER) {
                    hexagonComponent->stateOwner.setState(registry, hexagon, std::make_shared<HexagonWorkerIdle>());
                }
            }
        }
    }



    void HexagonWorkerIdle::draw(cocos2d::DrawNode* renderer, entt::registry& registry, entt::entity hexagon) {
        if(auto* hexagonRole = registry.try_get<HexagonRole>(hexagon); hexagonRole) {
            if(hexagonRole->role != Role::WORKER) return;

            Hexagon& hexagonComponent = registry.get<Hexagon>(hexagon);

            GameData& gameData = registry.ctx<GameData>();

            float hexagonFillSize = gameData.hexagonSize;
            if(hexagonRole->incomePeriod > 0.01f) {
                hexagonFillSize = m_elapsedTime / hexagonRole->incomePeriod * gameData.hexagonSize;
            }

            auto fillVertices = generateHexagonVertices(hexagonFillSize,
                                                        hexToRectCoords(hexagonComponent.position,
                                                                        gameData.hexagonSize));
            auto borderVertices = generateHexagonVertices(gameData.hexagonSize,
                                                          hexToRectCoords(hexagonComponent.position,
                                                                          gameData.hexagonSize));

            renderer->drawPolygon(fillVertices.data(), 6, getTeamColor(hexagonComponent.team), 0.0f, cocos2d::Color4F::BLACK);
            renderer->drawPolygon(borderVertices.data(), 6, cocos2d::Color4F(0, 0, 0, 0), Constants::BORDER_SIZE, cocos2d::Color4F(cocos2d::Color3B(getTeamColor(hexagonComponent.team) * 0.5f)));
        }
    }

    void HexagonWorkerIdle::update(entt::registry& registry, entt::dispatcher& dispatcher, entt::entity hexagon, float delta) {
        m_elapsedTime += delta;
        if(auto* hexagonRole = registry.try_get<HexagonRole>(hexagon); hexagonRole) {
            if(hexagonRole->role != Role::WORKER) return;

            if(m_elapsedTime > hexagonRole->incomePeriod) {
                //dispatcher.trigger<onWorkerHasIncome>(hexagon);
                m_elapsedTime = 0.0f;
            }
        }
    }


}
