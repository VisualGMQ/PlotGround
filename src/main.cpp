#include "context.hpp"
#include "pch.hpp"
#include <ctime>
#include <filesystem>

#ifdef __EMSCRIPTEN__
void MainLoop() {
    Context::GetInst().Update();
}
#endif

int main(int argc, char** argv) {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    tm* tm = std::localtime(&now_time);

    Context::Init(argc, argv);
    Context& ctx = Context::GetInst();

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(MainLoop, 0, true);
#else

    while (!ctx.ShouldExit()) {
        ctx.Update();
    }
#endif
    Context::Destroy();

    return 0;
}
