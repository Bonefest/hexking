#include "MapGenerator.h"

#include <vector>

namespace hk {

    GameMap MapSymmetricGenerator::generateMap(entt::registry& registry) {

        int32_t max_q = 8, max_r = 8;
        GameMap generatedMap(map_size{max_q * 4, max_r * 4});

        std::vector<cocos2d::Vec2> generatedPart;

        int32_t current_q = 0, current_r = 0;
        uint32_t counter = 0;

        uint32_t direction = 0;

        auto is_available = [&](int32_t q, int32_t r)->bool {
            if(r >= 0 && r <= max_r && q >= -max_q && q <= max_q) {

                    return true;
            }

            return false;
        };

        generatedPart.push_back(cocos2d::Vec2(current_q, current_r));
        while(counter < 150) {
            direction = cocos2d::random(0, 5);
            switch(direction) {
            case 0: //TOP-RIGHT
                {
                    if(is_available(current_q, current_r - 1)) {
                        current_r -= 1;
                        current_q += 1;

                        break;
                    }
                }

            case 1: //BOTTOM-RIGHT
                {
                    if(is_available(current_q + 1, current_r)) {
                        current_q += 1;

                        break;
                    }

                }

            case 2: //BOTTOM
                {
                    if(is_available(current_q, current_r + 1)) {
                        current_r += 1;

                        break;
                    }
                }

            case 3: //BOTTOM-LEFT
                {
                    if(is_available(current_q - 1, current_r + 1)) {
                        current_q -= 1;
                        current_r += 1;

                        break;
                    }
                }

            case 4: //TOP-LEFT
                {
                    if(is_available(current_q - 1, current_r)) {
                        current_q -= 1;

                        break;
                    }

                }
            case 5: //TOP
                {
                    if(is_available(current_q, current_r - 1)) {
                        current_r -= 1;

                        break;
                    }

                }
            }

            generatedPart.push_back(cocos2d::Vec2(current_q, current_r));
            counter++;

        }

        for(cocos2d::Vec2 pos: generatedPart) {
            cocos2d::Vec2 resPos = pos;
            if(!generatedMap.hasHexagon(resPos)) {
                auto hexagon = registry.create();
                Hexagon hexagonComponent(resPos);
                hexagonComponent.stateOwner.setState(registry, hexagon, std::make_shared<hk::HexagonIdle>());
                registry.assign<Hexagon>(hexagon, hexagonComponent);
                generatedMap.setHexagon(resPos, hexagon);

                cocos2d::log("%f %f", resPos.x, resPos.y);

                cocos2d::Vec2 symmPos = cocos2d::Vec2(resPos.x, -resPos.y);
                if(symmPos == resPos) continue;

                auto symmHexagon = registry.create();
                Hexagon symmHexagonComponent(symmPos);
                symmHexagonComponent.stateOwner.setState(registry, symmHexagon, std::make_shared<hk::HexagonIdle>());
                registry.assign<Hexagon>(symmHexagon, symmPos);
                generatedMap.setHexagon(symmPos, symmHexagon);
            }
        }

        return generatedMap;

    }

}
