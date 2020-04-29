#ifndef COMMAND_H_INCLUDED
#define COMMAND_H_INCLUDED

#include "../Dependencies/entt.hpp"

#include "../Components/Components.h"


namespace hk {

    class ICommand {
    public:
        virtual void execute(entt::registry& registry,
                             entt::dispatcher& dispatcher,
                             entt::entity target,
                             entt::entity sender) = 0;
    };

    class UpgradeCommand: public ICommand {
    public:
        void execute(entt::registry& registry,
                     entt::dispatcher& dispatcher,
                     entt::entity target,
                     entt::entity sender);
    };

    class PurchaseCommand: public ICommand {
    public:
        PurchaseCommand(Role t_role);

        virtual void execute(entt::registry& registry,
                             entt::dispatcher& dispatcher,
                             entt::entity target,
                             entt::entity sender);
    private:
        Role m_role;
    };

    class AttackCommand: public ICommand {
    public:
        virtual void execute(entt::registry& registry,
                             entt::dispatcher& dispatcher,
                             entt::entity target,
                             entt::entity sender);

    private:
        void attack(entt::registry& registry,
                    entt::dispatcher& dispatcher,
                    entt::entity target,
                    entt::entity striker,
                    float damage);
    };
}

#endif // COMMAND_H_INCLUDED
