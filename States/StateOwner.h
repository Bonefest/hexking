#ifndef STATEOWNER_H_INCLUDED
#define STATEOWNER_H_INCLUDED

#include "../Dependencies/entt.hpp"
#include "cocos2d.h"

#include "States.h"

namespace hk {

    class StateOwner {
    public:
        StateOwner();

        void setState(entt::registry& registry, entt::entity hexagon, std::shared_ptr<IHexagonState> state);

        void draw(cocos2d::DrawNode* drawNode, entt::registry& registry, entt::entity hexagon);
        void update(entt::registry& registry, entt::dispatcher& dispatcher, entt::entity hexagon, float delta);

    private:
        std::shared_ptr<IHexagonState> m_currentState;
    };

}


#endif // STATEOWNER_H_INCLUDED
