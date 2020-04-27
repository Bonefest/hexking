#ifndef EVENTS_H_INCLUDED
#define EVENTS_H_INCLUDED

#include "Dependencies/entt.hpp"
#include "cocos2d.h"

#include <memory>

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


    struct CommandEvent {
        CommandEvent(std::shared_ptr<ICommand> t_command, entt::entity t_target, entt::entity t_sender): command(t_command),
                                                                                                    target(t_target),
                                                                                                    sender(t_sender) { }

        std::shared_ptr<ICommand> command;
        entt::entity target;
        entt::entity sender;
    };


}
#endif // EVENTS_H_INCLUDED
