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
                //calculate price
                int price = -1;
                if(hexagonRole->level >= 1 && hexagonRole->level < 6 && player.resources > price) {
                    hexagonRole->level++;
                    player.resources -= price;
                    calculateHexagonData(*hexagonRole);
                }
            }

            cocos2d::log("here");
        }
    };

}

#endif // COMMAND_H_INCLUDED
