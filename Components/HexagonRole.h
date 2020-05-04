#ifndef HEXAGONROLE_H_INCLUDED
#define HEXAGONROLE_H_INCLUDED

#include <string>

namespace hk {

    enum class Role { ATTACKER, DEFENDER, WORKER };

    struct HexagonRole {
        explicit HexagonRole(Role role, int level);

        Role role;

        float hp;
        float currentHp;
        float regenerationSpeed;

        float damage;
        float attackCooldown;
        float attackPeriod;

        float income;
        float incomePeriod;

        int level;
    };

    void calculateHexagonData(HexagonRole& hexagonRole);
    float calculateHexagonImproveValue(HexagonRole& hexagonRole);
    float calculateHexagonBuildSize(Role role);

    std::string roleToSpriteName(Role role);
}

#endif // HEXAGONROLE_H_INCLUDED
