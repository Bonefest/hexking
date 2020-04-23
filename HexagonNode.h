#ifndef HEXAGONNODE_H_INCLUDED
#define HEXAGONNODE_H_INCLUDED

#include "cocos2d.h"

#include "helper.h"

namespace hk {

    class HexagonNode: public cocos2d::Node {
    public:
        static HexagonNode* createHexagon(float size) {
            HexagonNode* node = new HexagonNode();
            if(node != nullptr && node->initWithHexSize(size)) {
                node->autorelease();
                return node;
            }

            delete node;
            node = nullptr;
            return nullptr;
        }

        bool initWithHexSize(float size) {
            if(!cocos2d::Node::init()) return false;

            m_stencil = cocos2d::DrawNode::create();
            m_borderRenderer = cocos2d::DrawNode::create();

            m_gradient = cocos2d::LayerGradient::create();
            m_gradient->setVector(cocos2d::Vec2(0, 1));

            m_clipper = cocos2d::ClippingNode::create();
            m_clipper->addChild(m_gradient);

            setHexagonSize(size);
            setColor(cocos2d::Color3B::WHITE);

            addChild(m_stencil);
            addChild(m_borderRenderer);
            addChild(m_clipper);

            return true;
        }

        virtual void setColor(const cocos2d::Color3B& color) {
            m_hexagonColor = color;
            cocos2d::log("here");
            calculateGradient();
        }

        virtual void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& mat, uint32_t flag) {
            cocos2d::Node::draw(renderer, mat, flag);

            m_borderRenderer->clear();
            auto vertices = generateHexagonVertices(m_hexagonSize, getPosition());
            m_borderRenderer->drawPoly(vertices.data(), 6, true, cocos2d::Color4F::GRAY);

        }

        void setHexagonSize(float size) {
            m_hexagonSize = size;
            setContentSize(calculateHexSize(size));
            calculateHexagonView();
        }

    private:
        void calculateHexagonView() {
            m_stencil->clear();
            auto vertices = generateHexagonVertices(m_hexagonSize, getContentSize() * 0.5f);
            m_stencil->drawPolygon(vertices.data(), 6, cocos2d::Color4F::WHITE, 0.0f, cocos2d::Color4F::WHITE);
            m_clipper->setStencil(m_stencil);
        }

        void calculateGradient() {
            m_gradient->setStartColor(m_hexagonColor);
            m_gradient->setEndColor(cocos2d::Color3B::RED);
            m_gradient->setStartOpacity(128);
            m_gradient->setEndOpacity(255);
        }

        cocos2d::Color3B m_hexagonColor;
        cocos2d::ClippingNode* m_clipper;
        cocos2d::LayerGradient* m_gradient;
        cocos2d::DrawNode* m_stencil;
        cocos2d::DrawNode* m_borderRenderer;

        float m_hexagonSize;
    };

}
#endif // HEXAGONNODE_H_INCLUDED
