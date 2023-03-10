#ifndef __Test__
#define __Test__

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <functional>
#include <iostream>
#include <string>

#include "../Camera.hpp"

namespace test {

    class Test {
        public:
            Test() {}
            virtual ~Test() {}

            virtual void processInput(GLFWwindow* window, float deltaTime) {}
            virtual void onUpdate(float deltaTime) {}
            virtual void onRender() {}
            virtual void onImGuiRender() {}

            void setCamera(Camera* cam) { camera = cam; }
            Camera* camera;
    };

    class TestMenu : public Test {
        public:
            TestMenu(Test*& currentTestPointer);

            void onImGuiRender() override;

            template <typename T> void registerTest(const std::string& name) {
                std::cout << "Registering test " << name << std::endl;
                tests.push_back(std::make_pair(name, [](){ return new T(); } ));
            }
        private:
            Test*& currentTest;
            std::vector<std::pair<std::string, std::function<Test*()>>> tests;
    };
}
#endif // __Test__
