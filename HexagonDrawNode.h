#ifndef HEXAGONDRAWNODE_H_INCLUDED
#define HEXAGONDRAWNODE_H_INCLUDED

#include "cocos2d.h"
#include "common.h"

namespace hk {

    class HexagonDrawNode: public cocos2d::DrawNode {
    public:
        CREATE_FUNC(HexagonDrawNode);

        static HexagonDrawNode* createNode() {
            return HexagonDrawNode::create();
        }

        bool init() {
            if(!cocos2d::DrawNode::init()) return false;

            initVertices();

            return true;
        }

        void drawHexagon(cocos2d::Vec2 position,
                         float size,
                         cocos2d::Color4F fillColor,
                         cocos2d::Color4F borderColor,
                         float borderSize = Constants::BORDER_SIZE) {

            cocos2d::Vec2 vertices[6];
            for(int i = 0; i < 6; ++i) {
                vertices[i] = m_flatHexagonVertices[i] * size + position;
            }

            drawPolygon(vertices, 6, fillColor, Constants::BORDER_SIZE, borderColor);
        }

    private:
        void initVertices() {
            for(int i = 0; i < 6; ++i) {
                float angle = (M_PI / 3.0f) * i;
                m_flatHexagonVertices[i] = cocos2d::Vec2(std::cos(angle),
                                                         std::sin(angle));
            }
        }

        cocos2d::Vec2 m_flatHexagonVertices[6];
    };

}
#endif // HEXAGONDRAWNODE_H_INCLUDED
