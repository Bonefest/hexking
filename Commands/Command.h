#ifndef COMMAND_H_INCLUDED
#define COMMAND_H_INCLUDED

#include "../Dependencies/entt.hpp"
#include "../Components/Components.h"

namespace hk {

    class ICommand {
    public:
        virtual void execute(entt::registry& registry, entt::dispatcher& dispatcher, entt::entity target, entt::entity sender) = 0;
    };

    class UpgradeCommand: public ICommand {
    public:
        void execute(entt::registry& registry, entt::dispatcher& dispatcher, entt::entity target, entt::entity sender) {
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
    };

    class PurchaseCommand: public ICommand {
    public:
        PurchaseCommand(Role t_role): m_role(t_role) { }

        virtual void execute(entt::registry& registry, entt::dispatcher& dispatcher, entt::entity target, entt::entity sender) {
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
    private:
        Role m_role;
    };
}

#endif // COMMAND_H_INCLUDED
