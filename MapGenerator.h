#ifndef MAPGENERATOR_H_INCLUDED
#define MAPGENERATOR_H_INCLUDED

#include "Dependencies/entt.hpp"
#include "GameMap.h"

namespace hk {

    class IMapGenerator {
    public:
        virtual GameMap generateMap(entt::registry& registry) = 0;
    };

    class MapSymmetricGenerator: public IMapGenerator {
    public:
        virtual GameMap generateMap(entt::registry& registry);

    };

}

#endif // MAPGENERATOR_H_INCLUDED
