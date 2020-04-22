#ifndef SYSTEMSMANAGER_H_INCLUDED
#define SYSTEMSMANAGER_H_INCLUDED

#include "../Dependencies/entt.hpp"
#include "System.h"
#include <queue>
#include <memory>

namespace hk {

    typedef std::pair<int, std::shared_ptr<ISystem>> prioritized_system;

    class SystemsManager {
    public:

        void updateSystems(float delta);

        void addSystem(std::shared_ptr<ISystem> system, int priority = 0);
        void removeSystem(std::shared_ptr<ISystem> system) { /* TODO */ }

        entt::registry& getRegistry();
        entt::dispatcher& getDispatcher();

    private:
        entt::registry m_registry;
        entt::dispatcher m_dispatcher;

        std::priority_queue<prioritized_system> m_systems;
    };

}

#endif // SYSTEMSMANAGER_H_INCLUDED
