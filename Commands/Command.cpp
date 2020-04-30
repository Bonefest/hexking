#include "Command.h"

#include "../Events/Events.h"
#include "../GameMap.h"
#include "../helper.h"

namespace hk {

    void UpgradeCommand::execute(entt::registry& registry,
                                 entt::dispatcher& dispatcher,
                                 entt::entity target,
                                 entt::entity sender) {
        if(!registry.valid(target) || !registry.valid(sender)) return;
        //IF CLIENT -> GENERATE COMMAND TO SERVER

        //ELSE IF SERVER -> EXECUTE
        if(!registry.has<Player>(sender)) return;

        Player& player = registry.get<Player>(sender);

        if(auto hexagonRole = registry.try_get<HexagonRole>(target); hexagonRole) {
            //calculate price -> calculateUpgradePrice(role, level);
            int price = -1;
            if(hexagonRole->level >= 1 && hexagonRole->level < 6 && player.resources > price) {
                hexagonRole->level++;
                player.resources -= price;
                calculateHexagonData(*hexagonRole);

                Hexagon& hexagonComponent = registry.get<Hexagon>(target);
                hexagonComponent.icon->setContentSize(hexagonComponent.icon->getContentSize() * 1.15f);
            }
        }
    }


    PurchaseCommand::PurchaseCommand(Role t_role): m_role(t_role) { }

    void PurchaseCommand::execute(entt::registry& registry,
                                  entt::dispatcher& dispatcher,
                                  entt::entity target,
                                  entt::entity sender) {
        if(!registry.valid(target) || !registry.valid(sender)) return;

        if(!registry.has<Player>(sender)) return;

        Player& player = registry.get<Player>(sender);

        int price = -1; //calculate price -> calculatePurchasePrice(role);
        if(player.resources >= price) {
            player.resources -= price;
            registry.assign<HexagonRole>(target, m_role, 1);

            Hexagon& hexagonComponent = registry.get<Hexagon>(target);
            hexagonComponent.team = player.team;
        }
    }


    void AttackCommand::execute(entt::registry& registry,
                                entt::dispatcher& dispatcher,
                                entt::entity target,
                                entt::entity sender) {
        if(!registry.valid(sender)) return;

        Hexagon& hexagonComponent = registry.get<Hexagon>(sender);
        HexagonRole& hexagonRoleComponent = registry.get<HexagonRole>(sender);

        GameMap& gameMap = registry.ctx<GameMap>();

        auto enemyNeighbours = gameMap.getNeighbours(registry, hexagonComponent.position);
        for(auto iter = enemyNeighbours.begin(); iter != enemyNeighbours.end();) {
            if(!registry.valid(*iter) ||
               !registry.has<HexagonRole>(*iter) ||
               registry.get<Hexagon>(*iter).team == hexagonComponent.team) {

                iter = enemyNeighbours.erase(iter);
                continue;
            }

            iter++;
        }

        if(enemyNeighbours.empty())

        std::sort(enemyNeighbours.begin(), enemyNeighbours.end(), [&](entt::entity left, entt::entity right)->bool {
            return (registry.get<HexagonRole>(left).role == Role::DEFENDER);
        });

        float damage = hexagonRoleComponent.damage;

        switch(hexagonRoleComponent.role) {
            case Role::ATTACKER: {
                float damageSeries[3] = {damage, damage * .25f, damage * .25f};

                for(std::size_t i = 0; i < std::min<std::size_t>(enemyNeighbours.size(), 3); i++) {
                    attack(registry, dispatcher, enemyNeighbours[i], sender, damageSeries[i]);
                }

                break;
            }

            case Role::DEFENDER:
            case Role::WORKER: {
                attack(registry, dispatcher, enemyNeighbours.front(), sender, damage);

                break;
            }
        }
    }
    void AttackCommand::attack(entt::registry& registry,
                               entt::dispatcher& dispatcher,
                               entt::entity target,
                               entt::entity striker,
                               float damage) {
        GameData& gameData = registry.ctx<GameData>();
        Hexagon& targetHexagonComponent = registry.get<Hexagon>(target);
        cocos2d::log("%u %f %f", targetHexagonComponent.team, targetHexagonComponent.position.x, targetHexagonComponent.position.y);
        HexagonRole& hexagonRoleComponent = registry.get<HexagonRole>(target);
        hexagonRoleComponent.currentHp -= damage;
        dispatcher.trigger<HexagonDamageCausedEvent>(target, striker);
        dispatcher.trigger<CreateFloatTextEvent>(cocos2d::StringUtils::format("-%.1f", damage),
                                                 hexToRectCoords(targetHexagonComponent.position, gameData.hexagonSize),
                                                 cocos2d::Color4B(getTeamColor(targetHexagonComponent.team)), 32.0f);
    }


}
