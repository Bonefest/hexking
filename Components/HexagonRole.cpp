#include "HexagonRole.h"
#include <cmath>

namespace hk {
    HexagonRole::HexagonRole(Role t_role, int t_level): role(t_role),
                                                            hp(0.0f),
                                                            currentHp(0.0f),
                                                            damage(0.0f),
                                                            income(0.0f),
                                                            incomePeriod(0.0f),
                                                            level(t_level) {
        calculateHexagonData(*this);
    }

    void calculateHexagonData(HexagonRole& hexagonRole) {
        int level = std::min(std::max(1, hexagonRole.level), 6);
        switch(hexagonRole.role) {
            case Role::ATTACKER: {
                hexagonRole.hp = level * 10.0f;
                hexagonRole.damage = level * 1.0f;
                hexagonRole.income = 0.0f;
                break;
            }

            case Role::DEFENDER: {
                hexagonRole.hp = level * 20.0f;
                hexagonRole.damage = level * 0.3f;
                hexagonRole.income = 0.0f;
                break;
            }

            case Role::WORKER: {
                hexagonRole.hp = level * 3.0f;
                hexagonRole.damage = level * 0.1f;
                hexagonRole.income = level * 10.0f;
                hexagonRole.incomePeriod = 2.1f - level * 0.1f;
                break;
            }
        }
    }

}
