#ifndef __TestClearColor__
#define __TestClearColor__

#include "Test.hpp"

namespace test {

    class TestClearColor : public Test {
        public:
            TestClearColor();
            ~TestClearColor();

            void onUpdate(float deltaTime) override;
            void onRender() override;
            void onImGuiRender() override;
        private:
            float clearColor[4];
    };
}
#endif // __TestClearColor__
