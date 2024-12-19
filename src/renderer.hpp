#pragma once
#include "draw_list.hpp"
#include "flags.hpp"
#include "pch.hpp"
#include "transform.hpp"
#include "window.hpp"

class Font;

class Renderer {
public:
    friend class Texture;
    friend class Context;

    explicit Renderer(Window& window);
    ~Renderer();

    void Clear(const Color&);
    void Update();
    void Present() const;
    void SetScale(const Vec2& scale) const;
    void DrawLine(const Vec2& p1, const Vec2& p2, const Color&);
    void DrawRect(const Rect&, const Color&);
    void DrawCircle(const Circle&, const Color&, uint32_t seg = 0);
    void FillRect(const Rect&, const Color&);
    void DrawText(const std::string& text, const Vec2& position,
                  const Font& font, const Color&);
    void DrawLineWithWidth(const Vec2& p1, const Vec2& p2, const Color&, float width);
    void FillCircle(const Circle& circle, const Color& color, int seg = 100);
    void FillTriangle(const Vec2& p1, const Vec2& p2, const Vec2& p3, const Color& color);

    operator bool() const;

private:
    SDL_Renderer* renderer_;

    void setColor(const Color& c);
};