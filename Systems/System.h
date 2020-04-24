#ifndef SYSTEM_H_INCLUDED
#define SYSTEM_H_INCLUDED


#include "../Dependencies/entt.hpp"
#include "cocos2d.h"


#include "../Components/Components.h"
#include "../helper.h"
#include "../common.h"

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
            auto worldContainer = cocos2d::Director::getInstance()->getRunningScene()->getChildByTag(Constants::Tags::ScrollWorldContainer);
            m_renderer = cocos2d::DrawNode::create();
            worldContainer->addChild(m_renderer);
        }

        virtual void update(entt::registry& registry, entt::dispatcher& dispatcher, float delta) {
            m_renderer->clear();

            GameData& gameData = registry.ctx<GameData>();
            gameDadata

            registry.view<Hexagon>().each([&](entt::entity entity, Hexagon& hexagon){
                auto vertices = generateHexagonVertices(gameData.hexagonSize,
                                                        hexToRectCoords(hexagon.position, gameData.hexagonSize));

                if(hexagon.team == Team::NO_TEAM) {
                    m_renderer->drawPoly(vertices.data(),
                                            6,
                                            true,
                                            cocos2d::Color4F::WHITE);
                } else {
                    auto fillColor = hexagon.displayColor;
                    auto borderColor = fillColor;
                    borderColor.a = 0.5f;

                    m_renderer->drawPolygon(vertices.data(),
                                            6,
                                            fillColor,
                                            1.0f,
                                            borderColor);
                }

            });
        }

    private:
        cocos2d::DrawNode* m_renderer;

        /*
            ? Loading hexagon vertices once and every frame controlling hexagon size. On resizing - recalculating model. ?
        */

    };

    typedef cocos2d::EventTouch::EventCode event_code;
    typedef std::pair<event_code, cocos2d::Touch> event_data;


    class InputHandlingSystem: public ISystem {
    public:
        virtual void onEnter(entt::registry& registry, entt::dispatcher& dispatcher) {
            auto worldContainer = cocos2d::Director::getInstance()->getRunningScene()->getChildByTag(Constants::Tags::ScrollWorldContainer);

            auto cocosEDispatcher = cocos2d::Director::getInstance()->getEventDispatcher();

            auto touchListener = cocos2d::EventListenerTouchOneByOne::create();

            touchListener->onTouchBegan = CC_CALLBACK_2(InputHandlingSystem::onTouchBegan, this);
            touchListener->onTouchMoved = CC_CALLBACK_2(InputHandlingSystem::onTouchMoved, this);
            touchListener->onTouchEnded = CC_CALLBACK_2(InputHandlingSystem::onTouchEnded, this);
            touchListener->onTouchCancelled = CC_CALLBACK_2(InputHandlingSystem::onTouchCancelled, this);

            cocosEDispatcher->addEventListenerWithSceneGraphPriority(touchListener, worldContainer);
        }

        virtual void update(entt::registry& registry, entt::dispatcher& dispatcher, float delta) {
            for(event_data ed : m_unprocessedEvents) {
                switch(ed.first) {
                    case event_code::BEGAN: {

                    }


                }
            }

            m_unprocessedEvents.clear();
        }

        bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event) {
            m_unprocessedEvents.push_back(event_data{event_code::BEGAN, cocos2d::Touch(*touch)});
            return true;
        }

        void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event) {

        }

        void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event) {

        }

        void onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event) {

        }

    private:
        std::vector<event_data> m_unprocessedEvents;
    };

}


#endif // SYSTEM_H_INCLUDED
