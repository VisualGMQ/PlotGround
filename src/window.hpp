#pragma once
#include "pch.hpp"
#include "math.hpp"

class Window {
public:
    friend class Renderer;
    friend class Context;

    Window(const std::string& title, int w, int h);
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&);
    Window& operator=(Window&&);
    ~Window();

    Vec2 GetSize() const;

    operator bool() const;

private:
    SDL_Window* window_;
};