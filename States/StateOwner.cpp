
#include "StateOwner.h"

#include <memory>

using std::make_shared;
using std::shared_ptr;

namespace hk {
	StateOwner::StateOwner(): m_currentState(make_shared<NoState>()) { }

	void StateOwner::setState(entt::registry& registry, entt::entity hexagon, shared_ptr<IHexagonState> state) {
		m_currentState->onEnd(registry, hexagon);
		m_currentState = state;
		m_currentState->onBegin(registry, hexagon);
	}

    void StateOwner::draw(cocos2d::DrawNode* drawNode, entt::registry& registry, entt::entity hexagon) {
        m_currentState->draw(drawNode, registry, hexagon);
    }

	void StateOwner::update(entt::registry& registry, entt::dispatcher& dispatcher, entt::entity hexagon, float delta) {
        m_currentState->update(registry, dispatcher, hexagon, delta);
	}
}
