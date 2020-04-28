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

            auto borderVertices = generateHexagonVertices(gameData.hexagonSize,
                                                          hexToRectCoords(hexagonComponent.position,
                                                                          gameData.hexagonSize));

            float t = (std::sin(std::min(m_elapsedTime / hexagonRole->incomePeriod, 1.0f) * M_PI * 2.0f) + 1.0f) * 0.25f + 0.5f;

            renderer->drawPolygon(borderVertices.data(), 6, cocos2d::Color4F(cocos2d::Color3B(getTeamColor(hexagonComponent.team) * t)), Constants::BORDER_SIZE, cocos2d::Color4F(cocos2d::Color3B(getTeamColor(hexagonComponent.team) * 0.40f)));
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





    void HexagonAttack::draw(cocos2d::DrawNode* renderer, entt::registry& registry, entt::entity hexagon) {
        GameData& gameData = registry.ctx<GameData>();
        Hexagon& hexagonComponent = registry.get<Hexagon>(hexagon);
        HexagonRole& hexagonRoleComponent = registry.get<HexagonRole>(hexagon);


        float t = 1.0f;

        if(m_attacking) {
            if(hexagonRoleComponent.attackPeriod > 0.01f) {
                t = std::pow(std::min(m_attackElapsedTime / hexagonRoleComponent.attackPeriod, 1.0f), 4.0f);
            }
        } else {
           if(hexagonRoleComponent.attackCooldown > 0.01f) {
                t = 1.0f - std::pow(std::min(m_cooldownElapsedTime / hexagonRoleComponent.attackCooldown, 1.0f), 2.0f);
           }
        }

        float hexagonSize = gameData.hexagonSize + gameData.hexagonSize * 0.25f * t;

        auto vertices = generateHexagonVertices(hexagonSize, hexToRectCoords(hexagonComponent.position, gameData.hexagonSize));

        renderer->drawPolygon(vertices.data(), 6, getTeamColor(hexagonComponent.team), Constants::BORDER_SIZE, cocos2d::Color4F(cocos2d::Color3B(getTeamColor(hexagonComponent.team) * 0.40f)));
    }

    void HexagonAttack::update(entt::registry& registry, entt::dispatcher& dispatcher, entt::entity hexagon, float delta) {
        //if don't have attacking component -> change to hexagon idle

        HexagonRole& hexagonRoleComponent = registry.get<HexagonRole>(hexagon);
        if(m_attacking) {
            m_attackElapsedTime += delta;
            if(m_attackElapsedTime > hexagonRoleComponent.attackPeriod) {
                //dispatcher.trigger<onHexagonAttackEvent>(hexagon);

                m_attackElapsedTime = 0.0f;
                m_attacking = false;
            }
        } else {
            m_cooldownElapsedTime += delta;
            if(m_cooldownElapsedTime > hexagonRoleComponent.attackCooldown) {

                m_cooldownElapsedTime = 0.0f;
                m_attacking = true;
            }
        }

    }
}
