#ifndef HEXAGONDRAWNODE_H_INCLUDED
#define HEXAGONDRAWNODE_H_INCLUDED

#include "cocos2d.h"

class HexagonDrawNode: public cocos2d::DrawNode {
public:
    CREATE_FUNC(HexagonDrawNode);

    static HexagonDrawNode* createNode() {
        return HexagonDrawNode::create();
    }
};

#endif // HEXAGONDRAWNODE_H_INCLUDED
