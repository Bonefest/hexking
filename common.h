#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

namespace hk {

    namespace Constants {
        enum Tags {
            ScrollWorldContainer = 1,

            WorldCamera,
        };

        const static char* DEFAULT_FONT = "fonts/arial.ttf";
        const static float BORDER_SIZE = 2.0f;
        const static int UI_LEVEL = 100;
    }

}

#endif // COMMON_H_INCLUDED
