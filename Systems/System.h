#ifndef SYSTEM_H_INCLUDED
#define SYSTEM_H_INCLUDED


#include "../Dependencies/entt.hpp"
#include "cocos2d.h"


#include "../Components/Components.h"
#include "../Events/Events.h"
#include "../GameMap.h"
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
        HexagonRenderingSystem(): m_elapsedTime(0.0f) { }

        virtual void onEnter(entt::registry& registry, entt::dispatcher& dispatcher) {
            auto runningScene = cocos2d::Director::getInstance()->getRunningScene();
            m_renderer = cocos2d::DrawNode::create();
            runningScene->addChild(m_renderer);
        }

        virtual void update(entt::registry& registry, entt::dispatcher& dispatcher, float delta) {
            m_elapsedTime += 5.0f * delta;
            m_renderer->clear();

            registry.view<Hexagon>().each([&](entt::entity entity, Hexagon& hexagon){
                hexagon.stateOwner.draw(m_renderer, registry, entity);
            });

            GameData& gameData = registry.ctx<GameData>();
            registry.view<Hexagon, FocusedHexagon>().each([&](entt::entity hexagon, Hexagon& hexagonComponent, FocusedHexagon& focusedHexagonComponent){
                auto vertices = generateHexagonVertices(gameData.hexagonSize + 1.0f, hexToRectCoords(hexagonComponent.position, gameData.hexagonSize));
                m_renderer->drawPolygon(vertices.data(), 6, cocos2d::Color4F(0, 0, 0, 0), 3.0f, cocos2d::Color4F(0.0f, 1.0f, 1.0f, (std::sin(m_elapsedTime) + 1.0f) * 0.25f + 0.5f));
            });
        }

    private:
        cocos2d::DrawNode* m_renderer;
        float m_elapsedTime;

        /*
            ? Loading hexagon vertices once and every frame controlling hexagon size. On resizing - recalculating model. ?
        */

    };

    class HexagonUpdatingSystem: public ISystem {
    public:
        virtual void update(entt::registry& registry, entt::dispatcher& dispatcher, float delta) {
            registry.view<Hexagon>().each([&](entt::entity hexagon, Hexagon& hexagonComponent){
                hexagonComponent.stateOwner.update(registry, dispatcher, hexagon, delta);
            });
        }
    };

    typedef cocos2d::EventTouch::EventCode event_code;
    typedef std::pair<event_code, cocos2d::Touch> event_data;


    class InputHandlingSystem: public ISystem {
    public:
        virtual void onEnter(entt::registry& registry, entt::dispatcher& dispatcher) {
            auto runningScene = cocos2d::Director::getInstance()->getRunningScene();

            auto cocosEDispatcher = cocos2d::Director::getInstance()->getEventDispatcher();

            auto touchListener = cocos2d::EventListenerTouchOneByOne::create();

            touchListener->onTouchBegan = CC_CALLBACK_2(InputHandlingSystem::onTouchBegan, this);
            touchListener->onTouchMoved = CC_CALLBACK_2(InputHandlingSystem::onTouchMoved, this);
            touchListener->onTouchEnded = CC_CALLBACK_2(InputHandlingSystem::onTouchEnded, this);
            touchListener->onTouchCancelled = CC_CALLBACK_2(InputHandlingSystem::onTouchCancelled, this);

            cocosEDispatcher->addEventListenerWithSceneGraphPriority(touchListener, runningScene);
        }

        virtual void update(entt::registry& registry, entt::dispatcher& dispatcher, float delta) {
            for(event_data ed : m_unprocessedEvents) {
                switch(ed.first) {
                    case event_code::BEGAN: {
                        dispatcher.trigger<TouchBeganEvent>(ed.second);
                        break;
                    }

                    case event_code::MOVED: {
                        dispatcher.trigger<TouchMovedEvent>(ed.second);

                        auto camera = cocos2d::Director::getInstance()->getRunningScene()->getDefaultCamera();
                        camera->setPosition(camera->getPosition() - ed.second.getDelta());

                        break;
                    }

                    case event_code::ENDED: {
                        dispatcher.trigger<TouchEndedEvent>(ed.second);

                        break;
                    }

                    case event_code::CANCELLED: {
                        dispatcher.trigger<TouchCancelledEvent>(ed.second);

                        break;
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
            m_unprocessedEvents.push_back(event_data{event_code::MOVED, cocos2d::Touch(*touch)});
        }

        void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event) {
            m_unprocessedEvents.push_back(event_data{event_code::ENDED, cocos2d::Touch(*touch)});
        }

        void onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event) {
            m_unprocessedEvents.push_back(event_data{event_code::CANCELLED, cocos2d::Touch(*touch)});
        }

    private:
        std::vector<event_data> m_unprocessedEvents;
    };

    class HexagonTouchHandleSystem: public ISystem {
    public:
        virtual void onEnter(entt::registry& registry, entt::dispatcher& dispatcher) {
            dispatcher.sink<TouchBeganEvent>().connect<&HexagonTouchHandleSystem::onTouchBegan>(*this);
            dispatcher.sink<TouchEndedEvent>().connect<&HexagonTouchHandleSystem::onTouchEnded>(*this);
            dispatcher.sink<TouchMovedEvent>().connect<&HexagonTouchHandleSystem::onTouchMoved>(*this);
            dispatcher.sink<TouchCancelledEvent>().connect<&HexagonTouchHandleSystem::onTouchCancelled>(*this);
        }

        virtual void update(entt::registry& registry, entt::dispatcher& dispatcher, float delta) {
            GameData& gameData = registry.ctx<GameData>();
            GameMap& gameMap = registry.ctx<GameMap>();
            for(auto event: m_unprocessedTouchBeganEvents) {
//                registry.clear<FocusedHexagon>();
//
//                cocos2d::Camera* camera = cocos2d::Director::getInstance()->getRunningScene()->getDefaultCamera();
//                cocos2d::Vec2 globalLocation = camera->getPosition();
//                globalLocation -= cocos2d::Director::getInstance()->getVisibleSize() * 0.5f;
//                globalLocation += event.touch.getLocation();
//
//
//                m_focusedHexagon = gameMap.getHexagonAtPixel(globalLocation, gameData.hexagonSize);
//                if(registry.valid(m_focusedHexagon)) {
//                    registry.assign<FocusedHexagon>(m_focusedHexagon);
//                    cocos2d::log("here");
//                }
            }

            for(auto event: m_unprocessedTouchMovedEvents) {


            }

            m_unprocessedTouchBeganEvents.clear();
            m_unprocessedTouchEndedEvents.clear();
            m_unprocessedTouchMovedEvents.clear();
            m_unprocessedTouchCancelledEvents.clear();
        }

        void onTouchBegan(const TouchBeganEvent& event) {
            m_unprocessedTouchBeganEvents.push_back(event);
        }

        void onTouchEnded(const TouchEndedEvent& event) {
            m_unprocessedTouchEndedEvents.push_back(event);
        }

        void onTouchMoved(const TouchMovedEvent& event) {
            m_unprocessedTouchMovedEvents.push_back(event);
        }

        void onTouchCancelled(const TouchCancelledEvent& event) {
            m_unprocessedTouchCancelledEvents.push_back(event);
        }
    private:
        std::vector<TouchBeganEvent> m_unprocessedTouchBeganEvents;
        std::vector<TouchEndedEvent> m_unprocessedTouchEndedEvents;
        std::vector<TouchMovedEvent> m_unprocessedTouchMovedEvents;
        std::vector<TouchCancelledEvent> m_unprocessedTouchCancelledEvents;

        entt::entity m_focusedHexagon;
        std::map<entt::entity, double> m_pressedHexagons;
    };

}


#endif // SYSTEM_H_INCLUDED
