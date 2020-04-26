#ifndef HEXAGONRENDERER_H_INCLUDED
#define HEXAGONRENDERER_H_INCLUDED

#include "helper.h"

class HexagonRenderer: public cocos2d::Layer {
public:
    CREATE_FUNC(HexagonRenderer);
    static HexagonRenderer* createRenderer() {
        return HexagonRenderer::create();
    }

    virtual bool init() {
        if(!cocos2d::Layer::init()) return false;

        createGL();
        return true;
    }

    void createGL() {
        glProgram = cocos2d::GLProgram::createWithFilenames("Shaders/vertex.glsl", "Shaders/fragment.glsl");

        auto programState = cocos2d::GLProgramState::create(glProgram);
        setGLProgramState(programState);

        auto size = cocos2d::Director::getInstance()->getWinSize();

        std::vector<float> vvertices;
        std::vector<float> ccolors;

        for(int i = 0;i < 6;i++) {
            vvertices.push_back(0.0f);
            vvertices.push_back(0.0f);

            vvertices.push_back(std::cos(hk::radians(i * 60)) * 24);
            vvertices.push_back(std::sin(hk::radians(i * 60)) * 24);


            vvertices.push_back(std::cos(hk::radians( (i + 1) % 6 * 60)) * 24);
            vvertices.push_back(std::sin(hk::radians( (i + 1) % 6 * 60)) * 24);

            ccolors.push_back(1.0f);
            ccolors.push_back(1.0f);
            ccolors.push_back(1.0f);

            ccolors.push_back(0.5f);
            ccolors.push_back(0.5f);
            ccolors.push_back(0.5f);

            ccolors.push_back(0.5f);
            ccolors.push_back(0.5f);
            ccolors.push_back(0.5f);
        }

        glGenVertexArrays(1, &vao);

        glGenBuffers(1, &vbo1);
        glGenBuffers(1, &vbo2);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo1);

        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vvertices.size(), vvertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(cocos2d::GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(cocos2d::GLProgram::VERTEX_ATTRIB_POSITION);

        glBindBuffer(GL_ARRAY_BUFFER, vbo2);

        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ccolors.size(), ccolors.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(cocos2d::GLProgram::VERTEX_ATTRIB_COLOR, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(cocos2d::GLProgram::VERTEX_ATTRIB_COLOR);

        glBindVertexArray(0);


    }

    void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& mat, uint32_t flags) {
        cocos2d::Node::draw(renderer, mat, flags);
        cocos2d::CustomCommand* command = new cocos2d::CustomCommand();
        command->init(_globalZOrder, mat, flags);
        command->func = CC_CALLBACK_0(HexagonRenderer::onDraw, this, mat, flags);
        cocos2d::Director::getInstance()->getRenderer()->addCommand(command);
    }

    void onDraw(const cocos2d::Mat4& mat, uint32_t flags) {
        getGLProgramState()->applyGLProgram(mat);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 18);
        CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, 18);

        glBindVertexArray(0);
    }


private:
    cocos2d::GLProgram* glProgram;

    GLuint program;
    GLuint vbo1, vbo2;
    GLuint vao;
};

#endif // HEXAGONRENDERER_H_INCLUDED
