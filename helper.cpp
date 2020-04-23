#include "cocos2d.h"
#include "helper.h"

namespace hk {

    cocos2d::Vec3 hexToCube(cocos2d::Vec2 hex) {
        float x = hex.x;
        float y = hex.y;
        float z = -x-y;

        return cocos2d::Vec3(x, y, z);
    }

    cocos2d::Vec2 cubeToHex(cocos2d::Vec3 cube) {
        return cocos2d::Vec2(cube.x, cube.y);
    }


    cocos2d::Vec2 hexToRectCoords(cocos2d::Vec2 hex, float hexSize) {
        cocos2d::Vec2 result(cocos2d::Vec2::ZERO);

        result.x = std::round(hex.x) * 1.5f * hexSize;
        result.y = std::round(hex.x) * 0.5f * std::sqrt(3) * hexSize +
                   std::round(hex.y) * std::sqrt(3) * hexSize;

        return result;
    }


    cocos2d::Vec3 roundCube(cocos2d::Vec3 cube) {
        float x = std::round(cube.x);
        float y = std::round(cube.y);
        float z = std::round(cube.z);

        float dx = std::fabs(cube.x - x);
        float dy = std::fabs(cube.y - y);
        float dz = std::fabs(cube.z - z);

        if(dx > dy && dx > dz) {
            x = -y-z;
        } else if(dy > dz) {
            y = -x-z;
        } else {
            z = -x-y;
        }

        return cocos2d::Vec3(x, y, z);
    }

    cocos2d::Vec2 roundHex(cocos2d::Vec2 hex) {
        return cubeToHex(roundCube(hexToCube(hex)));
    }


    cocos2d::Vec2 rectToHexCoords(cocos2d::Vec2 pixelPosition, float hexSize) {
        float q = (2.0f / 3.0f) * pixelPosition.x / hexSize;
        float r = ((-1.0 / 3.0) * pixelPosition.x +
                  std::sqrt(3.0f)/3.0f * pixelPosition.y) / hexSize;
        return roundHex(cocos2d::Vec2(q, r));
    }

    float radians(float degrees) {
        return M_PI / 180.0f * degrees;
    }

    std::vector<cocos2d::Vec2> generateHexagonVertices(float size, cocos2d::Vec2 position) {
        std::vector<cocos2d::Vec2> vertices;
        for(int i = 0; i < 6; ++i) {
            vertices.push_back(cocos2d::Vec2(std::cos(radians(i * 60.0f)),
                                             std::sin(radians(i * 60.0f))) * size + position);
        }

        return vertices;
    }

    cocos2d::Size calculateHexSize(float size) {
        return cocos2d::Size(2.0f * size, std::sqrt(3) * size);
    }
}
