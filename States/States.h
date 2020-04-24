#ifndef STATES_H_INCLUDED
#define STATES_H_INCLUDED

#include "../Dependencies/entt.hpp"
#include "cocos2d.h"

namespace hk {

    class IHexagonState {
    public:
        virtual void onBegin(entt::registry&, entt::entity hexagon) { }
        virtual void draw(cocos2d::DrawNode* renderer, entt::registry& registry, entt::entity hexagon) { }
        virtual void update(entt::registry& registry, entt::entity hexagon, float delta) { }
        virtual void onEnd(entt::registry&, entt::entity hexagon) { }
    };

    class NoState: public IHexagonState { };

    class HexagonIdle: public IHexagonState {
    public:
        virtual void onBegin(entt::registry& registry, entt::entity hexagon);
        virtual void draw(cocos2d::DrawNode* renderer, entt::registry& registry, entt::entity hexagon);
        virtual void update(entt::registry& registry, entt::entity hexagon, float delta) { /* TODO */ }
    };

    class HexagonWorkerIdle: public IHexagonState {

    };
}

#endif // STATES_H_INCLUDED
