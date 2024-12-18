#include "draw_list.hpp"

#include "context.hpp"
#include "flags.hpp"
#include "log.hpp"
#include "macro.hpp"

void DrawList::SortByOrder() {
    std::stable_sort(drawCmds_.begin(), drawCmds_.end(),
              [](const DrawCmd& lhs, const DrawCmd& rhs) {
                  return lhs.order < rhs.order;
              });
}

void DrawList::Clear() {
    drawCmds_.clear();
    drawDatas_.clear();
}

void DrawList::PushClearCmd(const Color& color) {
    DrawCmd cmd{DrawCmd::Type::Clear};
    cmd.clear.color = color;

    drawCmds_.push_back(cmd);
}

void DrawList::PushLineDrawCmd(const Vec2& p1, const Vec2& p2,
                               const Color& color, float order) {
    DrawCmd cmd{DrawCmd::Type::Geom};
    cmd.geom.type = GeomDrawCmd::Type::Line;
    cmd.geom.elemCount = 1;
    cmd.geom.color = color;
    cmd.order = order;
    cmd.beginIdx = drawDatas_.size();

    drawCmds_.push_back(cmd);
    drawDatas_.push_back(p1.x);
    drawDatas_.push_back(p1.y);
    drawDatas_.push_back(p2.x);
    drawDatas_.push_back(p2.y);
}

void DrawList::PushLineStripDrawCmd(const Vec2* pts, uint32_t count,
                                    const Color& color, float order) {
    DrawCmd cmd{DrawCmd::Type::Geom};
    cmd.geom.type = GeomDrawCmd::Type::LineStrip;
    cmd.geom.elemCount = count;
    cmd.geom.color = color;
    cmd.order = order;
    cmd.beginIdx = drawDatas_.size();

    drawCmds_.push_back(cmd);
    for (uint32_t i = 0; i < count; i++) {
        drawDatas_.push_back(pts[i].x);
        drawDatas_.push_back(pts[i].y);
    }
}

void DrawList::PushLineLoopDrawCmd(const Vec2* pts, uint32_t count,
                                   const Color& color, float order) {
    DrawCmd cmd{DrawCmd::Type::Geom};
    cmd.geom.type = GeomDrawCmd::Type::LineLoop;
    cmd.geom.elemCount = count;
    cmd.geom.color = color;
    cmd.order = order;
    cmd.beginIdx = drawDatas_.size();

    drawCmds_.push_back(cmd);
    for (uint32_t i = 0; i < count; i++) {
        drawDatas_.push_back(pts[i].x);
        drawDatas_.push_back(pts[i].y);
    }
}

void DrawList::PushRectDrawCmd(const Vec2& topleft, const Vec2& size,
                               const Color& color, float order) {
    DrawCmd cmd{DrawCmd::Type::Geom};
    cmd.geom.type = GeomDrawCmd::Type::Rect;
    cmd.geom.elemCount = 1;
    cmd.geom.color = color;
    cmd.geom.fill = false;
    cmd.order = order;
    cmd.beginIdx = drawDatas_.size();

    drawCmds_.push_back(cmd);
    drawDatas_.push_back(topleft.x);
    drawDatas_.push_back(topleft.y);
    drawDatas_.push_back(size.w);
    drawDatas_.push_back(size.h);
}

void DrawList::PushRectFillCmd(const Vec2& topleft, const Vec2& size,
                               const Color& color, float order) {
    DrawCmd cmd{DrawCmd::Type::Geom};
    cmd.geom.type = GeomDrawCmd::Type::Rect;
    cmd.geom.elemCount = 1;
    cmd.geom.color = color;
    cmd.geom.fill = true;
    cmd.order = order;
    cmd.beginIdx = drawDatas_.size();

    drawCmds_.push_back(cmd);
    drawDatas_.push_back(topleft.x);
    drawDatas_.push_back(topleft.y);
    drawDatas_.push_back(size.w);
    drawDatas_.push_back(size.h);
}

void DrawList::PushCircleDrawCmd(const Vec2& center, float radius,
                                 const Color& color, float order) {
    DrawCmd cmd{DrawCmd::Type::Geom};
    cmd.geom.type = GeomDrawCmd::Type::Circle;
    cmd.geom.elemCount = 1;
    cmd.geom.color = color;
    cmd.order = order;
    cmd.beginIdx = drawDatas_.size();

    drawCmds_.push_back(cmd);
    drawDatas_.push_back(center.x);
    drawDatas_.push_back(center.y);
    drawDatas_.push_back(radius);
}

void DrawList::Execute(SDL_Renderer* renderer) const {
    for (auto& cmd : drawCmds_) {
        switch (cmd.type) {
            case DrawCmd::Type::Clear:
                setDrawColor(renderer, cmd.clear.color);
                SDL_RenderClear(renderer);
                break;
            case DrawCmd::Type::Geom:
                executeGeomCmd(renderer, cmd.geom, cmd.beginIdx);
                break;
        }
    }
}

void DrawList::setDrawColor(SDL_Renderer* renderer, const Color& c) const {
    SDL_SetRenderDrawColor(renderer, c.r * 255, c.g * 255, c.b * 255,
                           c.a * 255);
}

void DrawList::executeGeomCmd(SDL_Renderer* renderer, const GeomDrawCmd& cmd,
                              size_t startDataIdx) const {
    setDrawColor(renderer, cmd.color);
    switch (cmd.type) {
        case GeomDrawCmd::Type::Unknown:
            LOGW("unknown geometry draw command");
            break;
        case GeomDrawCmd::Type::Line:
            SDL_RenderDrawLineF(renderer, drawDatas_[startDataIdx],
                                drawDatas_[startDataIdx + 1],
                                drawDatas_[startDataIdx + 2],
                                drawDatas_[startDataIdx + 3]);
            break;
        case GeomDrawCmd::Type::LineStrip:
            SDL_RenderDrawLinesF(renderer,
                                 (SDL_FPoint*)&drawDatas_[startDataIdx],
                                 cmd.elemCount);
            break;
        case GeomDrawCmd::Type::LineLoop:
            SDL_RenderDrawLinesF(renderer,
                                 (SDL_FPoint*)&drawDatas_[startDataIdx],
                                 cmd.elemCount);
            SDL_RenderDrawLine(renderer, drawDatas_[startDataIdx],
                               drawDatas_[startDataIdx + 1],
                               drawDatas_[startDataIdx + cmd.elemCount - 1],
                               drawDatas_[startDataIdx + cmd.elemCount]);

            break;
        case GeomDrawCmd::Type::Rect:
            if (cmd.fill) {
                SDL_RenderFillRectsF(renderer,
                                     (SDL_FRect*)&drawDatas_[startDataIdx],
                                     cmd.elemCount);
            } else {
                SDL_RenderDrawRectsF(renderer,
                                     (SDL_FRect*)&drawDatas_[startDataIdx],
                                     cmd.elemCount);
            }
            break;
        case GeomDrawCmd::Type::Circle:
            for (size_t i = 0; i < cmd.elemCount; i++) {
                size_t idx = i * 3 + startDataIdx;
                Circle c{
                    Vec2{drawDatas_[idx], drawDatas_[idx + 1]},
                    drawDatas_[idx + 2]
                };
                constexpr int step = 10;
                constexpr float angle = 2 * PI / step;

                for (int j = 0; j < step; j++) {
                    float curAngle = j * angle;
                    float nextAngle = (j + 1) * angle;
                    Vec2 offset1{std::cos(curAngle), std::sin(curAngle)};
                    offset1 *= c.radius;
                    Vec2 offset2{std::cos(nextAngle), std::sin(nextAngle)};
                    offset2 *= c.radius;

                    Vec2 p1 = c.center + offset1;
                    Vec2 p2 = c.center + offset2;
                    SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
                }
            }
            break;
    }
}