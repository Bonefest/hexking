#ifndef EVENTS_H_INCLUDED
#define EVENTS_H_INCLUDED

#include "Dependencies/entt.hpp"
#include "cocos2d.h"

namespace hk {
    struct TouchBeganEvent {
        explicit TouchBeganEvent(cocos2d::Touch t_touch): touch(t_touch) { }

        cocos2d::Touch touch;
    };

    struct TouchMovedEvent {
        explicit TouchMovedEvent(cocos2d::Touch t_touch): touch(t_touch) { }

        cocos2d::Touch touch;
    };

    struct TouchEndedEvent {
        explicit TouchEndedEvent(cocos2d::Touch t_touch): touch(t_touch) { }

        cocos2d::Touch touch;
    };

    struct TouchCancelledEvent {
        explicit TouchCancelledEvent(cocos2d::Touch t_touch): touch(t_touch) { }

        cocos2d::Touch touch;
    };


    struct HexagonPressedEvent {
        HexagonPressedEvent(cocos2d::Touch t_touch, entt::entity t_hexagon): touch(t_touch),
                                                                             hexagon(t_hexagon) { }

        cocos2d::Touch touch;
        entt::entity hexagon;
    };

    struct HexagonReleasedEvent {
        HexagonReleasedEvent(cocos2d::Touch t_touch, entt::entity t_hexagon): touch(t_touch),
                                                                              hexagon(t_hexagon) { }

        cocos2d::Touch touch;
        entt::entity hexagon;
    };

    struct HexagonCancelledEvent {
        HexagonCancelledEvent(cocos2d::Touch t_touch, entt::entity t_hexagon): touch(t_touch),
                                                                               hexagon(t_hexagon) { }

        cocos2d::Touch touch;
        entt::entity hexagon;
    };

    struct ShowHexagonMenuEvent {
        ShowHexagonMenuEvent(entt::entity t_hexagon): hexagon(t_hexagon) { }

        entt::entity hexagon;
    };

    enum class MenuButtonType {
        UPGRADE,
        BUY_ATTACKER,
        BUY_DEFENDER,
        BUY_WORKER,
        CANCEL
    };

    struct PressedMenuButttonEvent {
        PressedMenuButttonEvent(entt::entity t_button, MenuButtonType t_type): button(t_button),
                                                                               type(t_type) { }

        entt::entity button;
        MenuButtonType type;
    };
}
#endif // EVENTS_H_INCLUDED
