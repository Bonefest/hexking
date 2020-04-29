#ifndef EVENTS_H_INCLUDED
#define EVENTS_H_INCLUDED

#include "Dependencies/entt.hpp"
#include "cocos2d.h"
#include <memory>

#include "../Commands/Command.h"

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

    struct HexagonDamageCausedEvent {
        HexagonDamageCausedEvent(entt::entity t_target, entt::entity t_striker): target(t_target),
                                                                                 striker(t_striker) { }

        entt::entity target;
        entt::entity striker;
    };

    struct CreateFloatTextEvent {
        CreateFloatTextEvent(const std::string& t_text,
                             cocos2d::Vec2 t_position,
                             cocos2d::Color4B t_color,
                             float t_size): text(t_text),
                                            position(t_position),
                                            color(t_color),
                                            size(t_size) { }

        std::string text;
        cocos2d::Vec2 position;
        cocos2d::Color4B color;
        float size;
    };

}
#endif // EVENTS_H_INCLUDED
