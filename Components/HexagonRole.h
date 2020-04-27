#ifndef HEXAGONROLE_H_INCLUDED
#define HEXAGONROLE_H_INCLUDED

namespace hk {

    enum class Role { ATTACKER, DEFENDER, WORKER };

    struct HexagonRole {
        explicit HexagonRole(Role role, int level);

        Role role;

        float hp;
        float currentHp;

        float damage;
        float income;
        float incomePeriod;

        int level;
    };

    void calculateHexagonData(HexagonRole& hexagonRole);
}

#endif // HEXAGONROLE_H_INCLUDED
