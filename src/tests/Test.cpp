#include "Test.hpp"

#include "imgui/imgui.h"

namespace test {

    TestMenu::TestMenu(Test*& currentTestPointer): currentTest(currentTestPointer) {
    }

    void TestMenu::onImGuiRender() {
        for (auto& test: tests) {
            if (ImGui::Button(test.first.c_str())) {
                // If button gets clicked, lambda in tests vector gets exectued creating new test
                currentTest = test.second();
            }
        }
    }
}

