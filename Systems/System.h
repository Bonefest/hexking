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
            registry.view<Hexagon, PressedHexagon>().each([&](entt::entity hexagon, Hexagon& hexagonComponent, PressedHexagon& pressedHexagonComponent){

                double t = std::min((getCurrentTimeInMs() - pressedHexagonComponent.pressingTime) / 1000.0, 1.0);

                auto vertices = generateHexagonVertices(t * gameData.hexagonSize, hexToRectCoords(hexagonComponent.position, gameData.hexagonSize));
                m_renderer->drawPolygon(vertices.data(), 6, cocos2d::Color4F(0.75f, 0.75f, 0.75f, (t + 1.0f) * 0.5f), 0.0f, cocos2d::Color4F::BLACK);
            });

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
        HexagonTouchHandleSystem() {
            m_pressedHexagon = std::pair{entt::null, 0.0};
        }

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
                auto hexagon = gameMap.getHexagonAtPixel(getTouchWorldLocation(event.touch), gameData.hexagonSize);
                if(registry.valid(hexagon)) {
                    m_pressedHexagons[event.touch.getID()] = hexagon;
                    dispatcher.trigger<HexagonPressedEvent>(event.touch, hexagon);
                }
            }

            for(auto event: m_unprocessedTouchMovedEvents) {
                auto hexagonIter = m_pressedHexagons.find(event.touch.getID());
                if(hexagonIter == m_pressedHexagons.end()) break;

                auto hexagon = gameMap.getHexagonAtPixel(getTouchWorldLocation(event.touch), gameData.hexagonSize);
                if(hexagonIter->second != hexagon) {
                    m_pressedHexagons.erase(event.touch.getID());
                    dispatcher.trigger<HexagonCancelledEvent>(event.touch, hexagon);
                }
            }

            for(auto event: m_unprocessedTouchEndedEvents) {
                auto hexagonIter = m_pressedHexagons.find(event.touch.getID());
                if(hexagonIter == m_pressedHexagons.end()) break;

                auto hexagon = gameMap.getHexagonAtPixel(getTouchWorldLocation(event.touch), gameData.hexagonSize);
                if(hexagonIter->second == hexagon) {
                    dispatcher.trigger<HexagonReleasedEvent>(event.touch, hexagon);
                }

                m_pressedHexagons.erase(event.touch.getID());
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
        cocos2d::Vec2 getTouchWorldLocation(cocos2d::Touch touch) {
            cocos2d::Camera* camera = cocos2d::Director::getInstance()->getRunningScene()->getDefaultCamera();
            cocos2d::Vec2 worldLocation = camera->getPosition();
            worldLocation -= cocos2d::Director::getInstance()->getVisibleSize() * 0.5f;
            worldLocation += touch.getLocation();

            return worldLocation;
        }

        std::vector<TouchBeganEvent> m_unprocessedTouchBeganEvents;
        std::vector<TouchEndedEvent> m_unprocessedTouchEndedEvents;
        std::vector<TouchMovedEvent> m_unprocessedTouchMovedEvents;
        std::vector<TouchCancelledEvent> m_unprocessedTouchCancelledEvents;

        std::pair<entt::entity, double> m_pressedHexagon;
        std::map<int, entt::entity> m_pressedHexagons;
    };

    class HexagonTouchActionSystem: public ISystem {
    public:
        virtual void onEnter(entt::registry& registry, entt::dispatcher& dispatcher) {
            dispatcher.sink<HexagonPressedEvent>().connect<&HexagonTouchActionSystem::onHexagonPressedEvent>(*this);
            dispatcher.sink<HexagonReleasedEvent>().connect<&HexagonTouchActionSystem::onHexagonReleasedEvent>(*this);
            dispatcher.sink<HexagonCancelledEvent>().connect<&HexagonTouchActionSystem::onHexagonCancelledEvent>(*this);
        }

        virtual void update(entt::registry& registry, entt::dispatcher& dispatcher, float delta) {
            for(auto event: m_unprocessedPressedEvents) {
                m_pressedHexagons[event.hexagon] = getCurrentTimeInMs();
                if(isFriendlyHexagon(registry, event.hexagon))
                    registry.assign_or_replace<PressedHexagon>(event.hexagon, getCurrentTimeInMs());
            }

            for(auto event: m_unprocessedReleasedEvents) {
                auto hexagonIter = m_pressedHexagons.find(event.hexagon);
                if(hexagonIter != m_pressedHexagons.end()) {
                    m_pressedHexagons.erase(hexagonIter);
                    registry.remove_if_exists<PressedHexagon>(event.hexagon);

                    dispatcher.trigger<ShowHexagonMenuEvent>(event.hexagon);
                }

            }

            for(auto event: m_unprocessedCanceledEvents) {
                auto hexagonIter = m_pressedHexagons.find(event.hexagon);
                if(hexagonIter != m_pressedHexagons.end()) {
                    registry.remove_if_exists<PressedHexagon>(event.hexagon);
                    registry.remove_if_exists<FocusedHexagon>(event.hexagon);

                    m_pressedHexagons.erase(hexagonIter);
                }
            }

            for(auto pressedHexagonIter = m_pressedHexagons.begin(); pressedHexagonIter != m_pressedHexagons.end();) {

                if(getCurrentTimeInMs() - pressedHexagonIter->second > 1000.0) {

                    if(registry.valid(pressedHexagonIter->first) && registry.has<PressedHexagon>(pressedHexagonIter->first)) {
                        cocos2d::log("trigger upgrade hexagon event!");
                        registry.remove<PressedHexagon>(pressedHexagonIter->first);

                        pressedHexagonIter = m_pressedHexagons.erase(pressedHexagonIter);
                    }
                } else {
                    pressedHexagonIter++;
                }
            }

            m_unprocessedPressedEvents.clear();
            m_unprocessedReleasedEvents.clear();
            m_unprocessedCanceledEvents.clear();
        }

        void onHexagonPressedEvent(const HexagonPressedEvent& event) {
            m_unprocessedPressedEvents.push_back(event);
        }

        void onHexagonReleasedEvent(const HexagonReleasedEvent& event) {
            m_unprocessedReleasedEvents.push_back(event);
        }

        void onHexagonCancelledEvent(const HexagonCancelledEvent& event) {
            m_unprocessedCanceledEvents.push_back(event);
        }

    private:
        std::vector<HexagonPressedEvent> m_unprocessedPressedEvents;
        std::vector<HexagonReleasedEvent> m_unprocessedReleasedEvents;
        std::vector<HexagonCancelledEvent> m_unprocessedCanceledEvents;

        std::map<entt::entity, double> m_pressedHexagons;
    };

    class HexagonMenuSystem: public ISystem {
    public:
        virtual void onEnter(entt::registry& registry, entt::dispatcher& dispatcher) {
            auto runningScene = cocos2d::Director::getInstance()->getRunningScene();

            m_menuRenderer = cocos2d::DrawNode::create();
            runningScene->addChild(m_menuRenderer);

            dispatcher.sink<ShowHexagonMenuEvent>().connect<&HexagonMenuSystem::onShowHexagonMenuEvent>(*this);
        }

        virtual void update(entt::registry& registry, entt::dispatcher& dispatcher, float delta) {
            m_menuRenderer->clear();

            GameData& gameData = registry.ctx<GameData>();
            GameMap& gameMap = registry.ctx<GameMap>();

            if(!m_unprocessedEvents.empty()) {
                registry.clear<FocusedHexagon>();

                auto hexagon = m_unprocessedEvents.back().hexagon;
                Hexagon& hexagonComponent = registry.get<Hexagon>(hexagon);
                if(hexagonComponent.team == gameData.controllableTeam ||
                   gameMap.hasFriendNeighbour(registry, hexagonComponent.position, gameData.controllableTeam)) {
                    //generate buttons
                    registry.assign_or_replace<FocusedHexagon>(hexagon);
                }

                m_unprocessedEvents.clear();
            }

            registry.view<Hexagon, FocusedHexagon>().each([&](entt::entity hexagon,
                                                              Hexagon& hexagonComponent,
                                                              FocusedHexagon& focusedComponent) {

                if(hexagonComponent.team == gameData.controllableTeam) {
                    cocos2d::log("show: upgrade button '+', and info inside other buttons depends on role");
                } else if(gameMap.hasFriendNeighbour(registry, hexagonComponent.position, gameData.controllableTeam)) {
                    cocos2d::log("show: 3 buttons - buy worker, buy attack and buy defender");
                } else {
                    cocos2d::log("show nothing");
                }
            });
        }

        void onShowHexagonMenuEvent(const ShowHexagonMenuEvent& event) {
            m_unprocessedEvents.push_back(event);
        }

        //onFocused start fade out and lerp
    private:
        cocos2d::DrawNode* m_menuRenderer;

        std::vector<ShowHexagonMenuEvent> m_unprocessedEvents;
    };
}


#endif // SYSTEM_H_INCLUDED
