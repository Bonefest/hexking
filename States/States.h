#ifndef STATES_H_INCLUDED
#define STATES_H_INCLUDED

#include "../Dependencies/entt.hpp"
#include "cocos2d.h"

namespace hk {

    class IHexagonState {
    public:
        virtual void onBegin(entt::registry&, entt::entity hexagon) { }
        virtual void draw(cocos2d::DrawNode* renderer, entt::registry& registry, entt::entity hexagon) { }
        virtual void update(entt::registry& registry, entt::dispatcher& dispatcher, entt::entity hexagon, float delta) { }
        virtual void onEnd(entt::registry&, entt::entity hexagon) { }
    };

    class NoState: public IHexagonState { };

    class HexagonIdle: public IHexagonState {
    public:
        virtual void onBegin(entt::registry& registry, entt::entity hexagon);
        virtual void draw(cocos2d::DrawNode* renderer, entt::registry& registry, entt::entity hexagon);
        virtual void update(entt::registry& registry, entt::dispatcher& dispatcher,  entt::entity hexagon, float delta);
    private:
        void findNextState(entt::registry& registry, entt::entity hexagon);
    };

    class HexagonWorkerIdle: public IHexagonState {
    public:
        virtual void draw(cocos2d::DrawNode* renderer, entt::registry& registry, entt::entity hexagon);
        virtual void update(entt::registry& registry, entt::dispatcher& dispatcher,  entt::entity hexagon, float delta);
    private:
        float m_elapsedTime;
    };

    class HexagonAttack: public IHexagonState {
    public:
        HexagonAttack(): m_attackElapsedTime(0.0f), m_cooldownElapsedTime(0.0f), m_attacking(true) { }
        virtual void draw(cocos2d::DrawNode* renderer, entt::registry& registry, entt::entity hexagon);
        virtual void update(entt::registry& registry, entt::dispatcher& dispatcher,  entt::entity hexagon, float delta);
    private:
        float m_attackElapsedTime;
        float m_cooldownElapsedTime;
        bool m_attacking;
    };
}

#endif // STATES_H_INCLUDED
