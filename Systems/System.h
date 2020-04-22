#ifndef SYSTEM_H_INCLUDED
#define SYSTEM_H_INCLUDED


#include "../Dependencies/entt.hpp"
#include "cocos2d.h"


#include "../Components/Components.h"
#include "../helper.h"


namespace hk {

    class ISystem {
    public:
        virtual void onEnter(entt::registry& registry, entt::dispatcher& dispatcher) { }
        virtual void update(entt::registry& registry, entt::dispatcher& dispatcher, float delta) { }
        virtual void onExit(entt::registry& registry, entt::dispatcher& dispatcher) { }
    };

    class HexagonRenderingSystem: public ISystem {
    public:
        virtual void onEnter(entt::registry& registry, entt::dispatcher& dispatcher) {
            auto runningScene = cocos2d::Director::getInstance()->getRunningScene();
            m_renderer = cocos2d::DrawNode::create();
            runningScene->addChild(m_renderer);
        }

        virtual void update(entt::registry& registry, entt::dispatcher& dispatcher, float delta) {

            GameData& gameData = registry.ctx<GameData>();



            registry.view<HexagonView>().each([&registry, &gameData](entt::entity hexagon, HexagonView& view){
                auto vertices = generateHexagonVertices(gameData.hexagonSize);

                m_renderer->drawPolygon(vertices.data(),
                                        6,
                                        view.fillColor,
                                        gameData.lineWidth,
                                        view.borderColor);
            });
        }

    private:
        cocos2d::DrawNode* m_renderer;

        /*
            ? Loading hexagon vertices once and every frame controlling hexagon size. On resizing - recalculating model. ?
        */

    };

}


#endif // SYSTEM_H_INCLUDED
