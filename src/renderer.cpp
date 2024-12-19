#include "renderer.hpp"

#include "font.hpp"
#include "log.hpp"
#include "macro.hpp"

Renderer::Renderer(Window& window) {
    renderer_ = SDL_CreateRenderer(window.window_, -1, 0);
    if (!renderer_) {
        LOGE("renderer create failed");
    }
}

Renderer::~Renderer() {
    SDL_DestroyRenderer(renderer_);
}

Renderer::operator bool() const {
    return renderer_;
}

void Renderer::setColor(const Color& c) {
    SDL_SetRenderDrawColor(renderer_, c.r * 255, c.g * 255, c.b * 255,
                           c.a * 255);
}

void Renderer::Clear(const Color& c) {
    setColor(c);
    SDL_RenderClear(renderer_);
}

void Renderer::Update() {}

void Renderer::Present() const {
    SDL_RenderPresent(renderer_);
}

void Renderer::DrawLine(const Vec2& p1, const Vec2& p2, const Color& c) {
    setColor(c);
    SDL_RenderDrawLineF(renderer_, p1.x, p1.y, p2.x, p2.y);
}

void Renderer::DrawRect(const Rect& r, const Color& c) {
    setColor(c);
    SDL_RenderDrawRectF(renderer_, (SDL_FRect*)&r);
}

void Renderer::FillRect(const Rect& r, const Color& c) {
    setColor(c);
    SDL_RenderFillRectF(renderer_, (SDL_FRect*)&r);
}

void Renderer::DrawText(const std::string& text, const Vec2& pos,
                        const Font& font, const Color& c) {
    SDL_Color color;
    color.r = c.r * 255;
    color.g = c.g * 255;
    color.b = c.b * 255;
    color.a = c.a * 255;
    SDL_Surface* surface =
        TTF_RenderUTF8_Blended(font.font_, text.c_str(), color);
    if (!surface) {
        LOGE("render text %s failed", text.c_str());
        return;
    }

    SDL_FRect dst;
    dst.x = pos.x;
    dst.y = pos.y;
    dst.w = surface->w;
    dst.h = surface->h;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        LOGE("create texture failed");
        return;
    }

    SDL_RenderCopyF(renderer_, texture, nullptr, &dst);
    SDL_DestroyTexture(texture);
}

bool rayIntersect(const Vec2& p1, const Vec2& dir1, const Vec2& p2, const Vec2& dir2, float& t1, float& t2) {
    Vec2 p12 = p2 - p1;
    float delta = dir1.x * (-dir2.y) - (-dir2.x * dir1.y);
    if (delta == 0) {
        return false;
    }

    t1 = p12.x * (-dir2.y) - (-dir2.x) * p12.y;
    t2 = dir1.x * p12.y - dir1.y * p12.x;
    return true;
}

void Renderer::DrawLineWithWidth(const Vec2& p1, const Vec2& p2, const Color& c,
                                 float width) {
    Vec2 d = p2 - p1;
    Vec2 n{d.y, -d.x};
    n = n.Normalize() * width;

    Vec2 pts[] = {
        p1 + n, p1 - n,
        p2 + n, p2 - n
    };
    int indices[] = {0, 1, 2, 1, 2, 3};
    SDL_Color color;
    color.r = c.r * 255;
    color.g = c.g * 255;
    color.b = c.b * 255;
    color.a = c.a * 255;

    SDL_RenderGeometryRaw(renderer_, nullptr, (float*)&pts, sizeof(Vec2),
                          &color, 0, nullptr, 0, 4, indices, 6,
                          4);
}

void Renderer::DrawCircle(const Circle& c, const Color& color, uint32_t seg) {
    setColor(color);
    seg = seg == 0 ? 20 : seg;
    float angle = 2 * PI / seg;

    for (int j = 0; j < seg; j++) {
        float curAngle = j * angle;
        float nextAngle = (j + 1) * angle;
        Vec2 offset1{std::cos(curAngle), std::sin(curAngle)};
        offset1 *= c.radius;
        Vec2 offset2{std::cos(nextAngle), std::sin(nextAngle)};
        offset2 *= c.radius;

        Vec2 p1 = c.center + offset1;
        Vec2 p2 = c.center + offset2;
        SDL_RenderDrawLine(renderer_, p1.x, p1.y, p2.x, p2.y);
    }
}

void Renderer::SetScale(const Vec2& scale) const {
    SDL_RenderSetScale(renderer_, scale.w, scale.h);
}