#ifndef SYSTEM_H_INCLUDED
#define SYSTEM_H_INCLUDED

#include "cocos2d.h"
#include "../Dependencies/entt.hpp"

namespace hk {

    class ISystem {
    public:
        void onEnter(entt::registry& registry, entt::dispatcher& dispatcher) { }
        void update(entt::registry& registry, entt::dispatcher& dispatcher, float delta) { }
        void onExit(entt::registry& registry, entt::dispatcher& dispatcher) { }
    };

    class HexagonRenderingSystem: public ISystem {
        void update(entt::registry& registry, entt::dispatcher& dispatcher, float delta) {
            cocos2d::log("here");
        }
    };

}


#endif // SYSTEM_H_INCLUDED
