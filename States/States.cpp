#include "States.h"

#include "../Components/Components.h"
#include "../Events/Events.h"
#include "../helper.h"
#include "../common.h"

namespace hk {

    void HexagonIdle::onBegin(entt::registry& registry, entt::entity hexagon) {
        findNextState(registry, hexagon);
    }

    void HexagonIdle::draw(HexagonDrawNode* renderer, entt::registry& registry, entt::entity hexagon) {
        GameData& gameData = registry.ctx<GameData>();
        Hexagon& hexagonComponent = registry.get<Hexagon>(hexagon);

        float hexagonFillSize = gameData.hexagonSize;
        if(auto hexagonRoleComponent = registry.try_get<HexagonRole>(hexagon); hexagonRoleComponent) {
            hexagonFillSize *=  calculateSize(*hexagonRoleComponent);
        }

        cocos2d::Color4F fillColor(0.0f, 0.0f, 0.0f, 0.0f);
        cocos2d::Color4F borderColor = cocos2d::Color4F::WHITE;
        if(hexagonComponent.team != Team::NO_TEAM) {
            fillColor = getTeamColor(hexagonComponent.team);
            borderColor = fillColor * 0.5f;
        }

        cocos2d::Vec2 hexagonRectPosition = hexToRectCoords(hexagonComponent.position, gameData.hexagonSize);

        renderer->drawHexagon(hexagonRectPosition,
                              hexagonFillSize,
                              fillColor,
                              cocos2d::Color4F::BLACK,
                              0);

        renderer->drawHexagon(hexagonRectPosition,
                              gameData.hexagonSize,
                              cocos2d::Color4F(cocos2d::Color3B(fillColor), 0.25f),
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







    void HexagonWorkerIdle::draw(HexagonDrawNode* renderer, entt::registry& registry, entt::entity hexagon) {
        if(auto* hexagonRole = registry.try_get<HexagonRole>(hexagon); hexagonRole) {
            if(hexagonRole->role != Role::WORKER) return;

            GameData& gameData = registry.ctx<GameData>();
            Hexagon& hexagonComponent = registry.get<Hexagon>(hexagon);
            HexagonRole& hexagonRoleComponent = registry.get<HexagonRole>(hexagon);

            float t = (std::sin(std::min(m_elapsedTime / hexagonRole->incomePeriod, 1.0f) * M_PI * 2.0f) + 1.0f) * 0.25f + 0.5f;
            cocos2d::Color4F fillColor = getTeamColor(hexagonComponent.team) * t;
            cocos2d::Color4F borderColor = fillColor * 0.5f;
            fillColor.a = borderColor.a = 1.0f;

            float hexagonFillSize = gameData.hexagonSize * calculateSize(hexagonRoleComponent);
            cocos2d::Vec2 hexagonRectPosition = hexToRectCoords(hexagonComponent.position, gameData.hexagonSize);

            renderer->drawHexagon(hexagonRectPosition,
                                  hexagonFillSize,
                                  fillColor,
                                  cocos2d::Color4F::BLACK,
                                  0.0f);

            renderer->drawHexagon(hexagonRectPosition,
                                  gameData.hexagonSize,
                                  cocos2d::Color4F(0, 0, 0, 0),
                                  borderColor);




        }
    }

    void HexagonWorkerIdle::update(entt::registry& registry, entt::dispatcher& dispatcher, entt::entity hexagon, float delta) {
        m_elapsedTime += delta;
        if(auto* hexagonRole = registry.try_get<HexagonRole>(hexagon); hexagonRole) {
            if(hexagonRole->role != Role::WORKER) return;

            if(m_elapsedTime > hexagonRole->incomePeriod) {
                //dispatcher.trigger<WorkerHasIncome>(hexagon);

                GameData& gameData = registry.ctx<GameData>();
                Hexagon& hexagonComponent = registry.get<Hexagon>(hexagon);
                Player& playerComponent = registry.get<Player>(gameData.players[hexagonComponent.team]);
                playerComponent.resources += hexagonRole->income;

                m_elapsedTime = 0.0f;
            }
        }
    }





    void HexagonAttack::draw(HexagonDrawNode* renderer, entt::registry& registry, entt::entity hexagon) {
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

        float hexagonSize = (gameData.hexagonSize + gameData.hexagonSize * 0.25f * t);
        cocos2d::Vec2 hexagonRectPosition = hexToRectCoords(hexagonComponent.position, gameData.hexagonSize);

        renderer->drawHexagon(hexagonRectPosition,
                              hexagonSize * calculateSize(hexagonRoleComponent),
                              getTeamColor(hexagonComponent.team),
                              cocos2d::Color4F(0, 0, 0, 0),
                              0.0f);

        renderer->drawHexagon(hexagonRectPosition,
                              hexagonSize,
                              cocos2d::Color4F(0, 0, 0, 0),
                              cocos2d::Color4F(cocos2d::Color3B(getTeamColor(hexagonComponent.team) * 0.40f)));
    }

    void HexagonAttack::update(entt::registry& registry, entt::dispatcher& dispatcher, entt::entity hexagon, float delta) {
        HexagonRole& hexagonRoleComponent = registry.get<HexagonRole>(hexagon);
        if(m_attacking) {
            m_attackElapsedTime += delta;
            if(m_attackElapsedTime > hexagonRoleComponent.attackPeriod) {
                dispatcher.trigger<CommandEvent>(std::make_shared<AttackCommand>(), entt::null, hexagon);

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

    float calculateSize(HexagonRole& hexagonRoleComponent) {
        return std::max(hexagonRoleComponent.currentHp / hexagonRoleComponent.hp, 0.1f);
    }
}
