#include "HexagonRole.h"
#include <cmath>

namespace hk {
    HexagonRole::HexagonRole(Role t_role, int t_level): role(t_role),
                                                            hp(0.0f),
                                                            currentHp(0.0f),
                                                            damage(0.0f),
                                                            attackCooldown(0.0f),
                                                            income(0.0f),
                                                            incomePeriod(0.0f),
                                                            level(t_level) {
        calculateHexagonData(*this);
        currentHp = hp;
    }

    void calculateHexagonData(HexagonRole& hexagonRole) {
        int level = std::min(std::max(1, hexagonRole.level), 6);

        hexagonRole.attackPeriod = 0.1f;
        switch(hexagonRole.role) {
            case Role::ATTACKER: {
                hexagonRole.hp = level * 10.0f;
                hexagonRole.damage = level * 1.0f;
                hexagonRole.attackCooldown = 1.2f - level * 0.15f;
                hexagonRole.income = 0.0f;
                break;
            }

            case Role::DEFENDER: {
                hexagonRole.hp = level * 20.0f;
                hexagonRole.damage = level * 0.3f;
                hexagonRole.attackCooldown = 1.6f - level * 0.15f;
                hexagonRole.income = 0.0f;
                break;
            }

            case Role::WORKER: {
                hexagonRole.hp = level * 3.0f;
                hexagonRole.damage = level * 0.1f;
                hexagonRole.attackCooldown = 2.2f - 0.1f*level;
                hexagonRole.income = level * 10.0f;
                hexagonRole.incomePeriod = 1.8f - level * 0.15f;
                break;
            }
        }
    }

    std::string roleToSpriteName(Role role) {
        switch(role) {
            case Role::ATTACKER: return "attacker.png";
            case Role::DEFENDER: return "defender.png";
            case Role::WORKER: return "worker.png";
        }

        return "";
    }
}
