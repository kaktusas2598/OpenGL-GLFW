#ifndef __Test__
#define __Test__

namespace test {

    class Test {
        public:
            Test() {}
            virtual ~Test() {}

            virtual void onUpdate(float deltaTime) {}
            virtual void onRender() {}
            virtual void onImGuiRender() {}
        private:
    };
}
#endif // __Test__
