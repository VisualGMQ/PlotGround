#pragma once

#include "app.hpp"
#include "args.hpp"
#include "input/keyboard.hpp"
#include "input/mouse.hpp"
#include "pch.hpp"
#include "renderer.hpp"
#include "timer.hpp"
#include "window.hpp"

class Context {
public:
    static Context& GetInst() {
        assert(inst);
        return *inst;
    }

    static void Init(int argc, char** argv);
    static void Destroy();

    std::unique_ptr<Window> window;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<Keyboard> keyboard;
    std::unique_ptr<Mouse> mouse;
    std::unique_ptr<Time> time;
    std::unique_ptr<App> app;
    std::unique_ptr<Args> args;

    void Update();

    void Exit() { shouldExit_ = true; }
    bool ShouldExit() const { return shouldExit_; }

private:
    SDL_Event event_;
    bool shouldExit_ = false;

    ~Context();

    void initSDL();
    void quitSDL();
    void initImGui();
    void quitImGui();
    void postInit(int argc, char** argv);
    void handleEvent();
    void initApp();

    static Context* inst;
};