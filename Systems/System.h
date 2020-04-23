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

        virtual void update(entt::registry& registry, entt::dispatcher& dispatcher, float delta) {

        }


        /*
            ? Loading hexagon vertices once and every frame controlling hexagon size. On resizing - recalculating model. ?
        */

    };


    typedef cocos2d::EventTouch::EventCode event_code;
    typedef std::pair<event_code, cocos2d::Touch> event_data;


    class InputHandlingSystem: public ISystem {
    public:
        virtual void onEnter(entt::registry& registry, entt::dispatcher& dispatcher) {
            auto cocosEDispatcher = cocos2d::Director::getInstance()->getEventDispatcher();

            auto touchListener = cocos2d::EventListenerTouchOneByOne::create();

            touchListener->onTouchBegan = CC_CALLBACK_2(InputHandlingSystem::onTouchBegan, this);
            touchListener->onTouchMoved = CC_CALLBACK_2(InputHandlingSystem::onTouchMoved, this);
            touchListener->onTouchEnded = CC_CALLBACK_2(InputHandlingSystem::onTouchEnded, this);
            touchListener->onTouchCancelled = CC_CALLBACK_2(InputHandlingSystem::onTouchCancelled, this);

            cocosEDispatcher->addEventListenerWithFixedPriority(touchListener, 1);
        }

        virtual void update(entt::registry& registry, entt::dispatcher& dispatcher, float delta) {
            for(event_data ed : m_unprocessedEvents) {
                switch(ed.first) {
                    case event_code::BEGAN: {


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
