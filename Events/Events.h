#ifndef EVENTS_H_INCLUDED
#define EVENTS_H_INCLUDED

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

}
#endif // EVENTS_H_INCLUDED
