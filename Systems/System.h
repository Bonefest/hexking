#ifndef SYSTEM_H_INCLUDED
#define SYSTEM_H_INCLUDED

#include "cocos2d.h"
#include "../Dependencies/entt.hpp"

namespace hk {

    class ISystem {
    public:
        virtual void onEnter(entt::registry& registry, entt::dispatcher& dispatcher) { }
        virtual void update(entt::registry& registry, entt::dispatcher& dispatcher, float delta) { }
        virtual void onExit(entt::registry& registry, entt::dispatcher& dispatcher) { }
    };

    class HexagonRenderingSystem: public ISystem {
        virtual void update(entt::registry& registry, entt::dispatcher& dispatcher, float delta) {

        }
    };

}


#endif // SYSTEM_H_INCLUDED
