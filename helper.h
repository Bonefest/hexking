#ifndef HELPER_H_INCLUDED
#define HELPER_H_INCLUDED

#include "cocos2d.h"

namespace hk {

    cocos2d::Vec3 hexToCube(cocos2d::Vec2 hex);

    cocos2d::Vec2 cubeToHex(cocos2d::Vec3 cube);


    cocos2d::Vec2 hexToRectCoords(cocos2d::Vec2 hex, float hexSize);

    cocos2d::Vec2 rectToHexCoords(cocos2d::Vec2 pixelPosition, float hexSize);


    cocos2d::Vec3 roundCube(cocos2d::Vec3 cube);

    cocos2d::Vec2 roundHex(cocos2d::Vec2 hex);


    float radians(float degrees);

    std::vector<cocos2d::Vec2> generateHexagonVertices(float size, cocos2d::Vec2 position);

    cocos2d::Size calculateHexSize(float size);
}

#endif // HELPER_H_INCLUDED
