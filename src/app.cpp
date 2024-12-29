#include "app.hpp"

#include "context.hpp"
#include "log.hpp"

#include <filesystem>
#include <fstream>

void PlotApp::OnInit() {
    font_ = Context::GetInst().fontMgr->Load("assets/arial.ttf", "arial");
    font_->SetPtSize(20);

    imFont_ = ImGui::GetIO().Fonts->AddFontFromFileTTF("assets/arial.ttf", 15, nullptr, nullptr);
    ImGui::GetIO().FontDefault = imFont_;
    
    auto& args = Context::GetInst().args->args;
    if (args.empty()) {
        std::cout << "you must provide data file" << std::endl;
        return;
    }
    
    std::ifstream file(args[0]);
    if (file.fail()) {
        LOGE("%s can't open", args[0].c_str());
        return;
    }

    int i = 0;
    while (!file.eof()) {
        LOGI("processing line %d", i++);
        char discard;
        float value;

        if (file.fail()) {
            LOGE("read failed");
            return;
        }

        file >> value;
        file >> discard;
        file >> value;
        xData_.push_back(value);
        file >> discard;
        file >> value;
        yData_.push_back(value);
        file >> discard;
        file >> value;
        zData_.push_back(value);
        file >> discard;
        file >> value;

        sumData_.push_back(std::sqrt(xData_.back() * xData_.back() +
                                     yData_.back() * yData_.back() +
                                     zData_.back() * zData_.back()));
    }
}

void PlotApp::OnUpdate(float deltaTime) {
    increaseRange(deltaTime);

    initStyle();

    if (ImGui::Begin("lines")) {
        setLineColor(ImColor{0, 0, 0});
        plotOnePolyline("B", sumData_);

        setLineColor(ImColor{255, 0, 0});
        plotOnePolyline("Bx", xData_);

        setLineColor(ImColor{0, 255, 0});
        plotOnePolyline("By", yData_);

        setLineColor(ImColor{0, 0, 255});
        plotOnePolyline("Bz", zData_);
        ImGui::End();
    }

    quitStyle();

    // ImPlot::ShowDemoWindow();

    float xAngle = 0;
    float zAngle = 0;
    float len = 0; 
    if (end_ >= 0 && end_ < xData_.size()) {
        float x = xData_[end_];
        float y = yData_[end_];
        float z = zData_[end_];

        len = std::sqrt(x * x + y * y + z * z);

        if (len < 50) {
            LOGI("here");
        }

        if (len != 0) {
            x /= len;
            y /= len;
            z /= len;
        }

        bool xNegative = y < 0;
        bool zNegative = y < 0;

        xAngle = std::acos(x);
        if (xNegative) {
            xAngle = 2.0 * PI - xAngle;
        }
        zAngle = std::acos(z);
        if (zNegative) {
            zAngle = 2.0 * PI - zAngle;
        }
    }

    drawClock("Φ=", "XY  Plane", Vec2{700, 400}, 250, xAngle, false, 0, false,
              "Y", "X");
    drawClock("θ=", "XZ  Plane", Vec2{700, 1000}, 250, zAngle, true, -PI * 0.5,
              true, "Z", "X");
    drawBLength(len, "|B|=", Vec2{1200, 400});

    if (ImGui::Begin("config")) {
        ImGui::DragFloat("speed", &speed_, 1, 0.001, std::numeric_limits<float>::max());
        ImGui::BeginDisabled(true);
        ImGui::DragFloat("curret data index", &end_, 1, 0, 0, "%.3f");
        ImGui::EndDisabled();
        ImGui::End();
    }
}

void PlotApp::initStyle() {
    ImPlot::PushColormap(ImPlotColormap_Deep);
    backupStyle_ = ImPlot::GetStyle();

    ImPlotStyle& style              = ImPlot::GetStyle();

    ImVec4* colors                  = style.Colors;
    colors[ImPlotCol_Line]          = IMPLOT_AUTO_COL;
    colors[ImPlotCol_Fill]          = IMPLOT_AUTO_COL;
    colors[ImPlotCol_MarkerOutline] = IMPLOT_AUTO_COL;
    colors[ImPlotCol_MarkerFill]    = IMPLOT_AUTO_COL;
    colors[ImPlotCol_ErrorBar]      = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImPlotCol_FrameBg]       = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
    colors[ImPlotCol_PlotBg]        = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);
    colors[ImPlotCol_PlotBorder]    = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImPlotCol_LegendBg]      = ImVec4(0.92f, 0.92f, 0.95f, 1.00f);
    colors[ImPlotCol_LegendBorder]  = ImVec4(0.80f, 0.81f, 0.85f, 1.00f);
    colors[ImPlotCol_LegendText]    = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImPlotCol_TitleText]     = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImPlotCol_InlayText]     = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImPlotCol_AxisText]      = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImPlotCol_AxisGrid]      = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImPlotCol_AxisBgHovered]   = ImVec4(0.92f, 0.92f, 0.95f, 1.00f);
    colors[ImPlotCol_AxisBgActive]    = ImVec4(0.92f, 0.92f, 0.95f, 0.75f);
    colors[ImPlotCol_Selection]     = ImVec4(1.00f, 0.65f, 0.00f, 1.00f);
    colors[ImPlotCol_Crosshairs]    = ImVec4(0.23f, 0.10f, 0.64f, 0.50f);

    style.LineWeight       = 1.5;
    style.Marker           = ImPlotMarker_None;
    style.MarkerSize       = 4;
    style.MarkerWeight     = 1;
    style.FillAlpha        = 1.0f;
    style.ErrorBarSize     = 5;
    style.ErrorBarWeight   = 1.5f;
    style.DigitalBitHeight = 8;
    style.DigitalBitGap    = 4;
    style.PlotBorderSize   = 0;
    style.MinorAlpha       = 1.0f;
    style.MajorTickLen     = ImVec2(0,0);
    style.MinorTickLen     = ImVec2(0,0);
    style.MajorTickSize    = ImVec2(0,0);
    style.MinorTickSize    = ImVec2(0,0);
    style.MajorGridSize    = ImVec2(1.2f,1.2f);
    style.MinorGridSize    = ImVec2(1.2f,1.2f);
    style.PlotPadding      = ImVec2(12,12);
    style.LabelPadding     = ImVec2(5,5);
    style.LegendPadding    = ImVec2(5,5);
    style.MousePosPadding  = ImVec2(5,5);
    style.PlotMinSize      = ImVec2(300,225);
}

void PlotApp::quitStyle() {
    ImPlot::GetStyle() = backupStyle_;
    ImPlot::PopColormap();
}

void PlotApp::increaseRange(float deltaTime) {
    if (end_ >= xData_.size()) {
        return;
    }
    
    if (end_ < count_) {
        end_ += deltaTime * speed_;
    } else if (end_ - begin_ >= count_) {
        end_ += deltaTime * speed_;
        begin_ += deltaTime * speed_;
    }
}

void PlotApp::plotOnePolyline(const char* title,
                              const std::vector<float>& data) {
    if (data.empty()) {
        return;
    }

    if (ImPlot::BeginPlot(title)) {
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, 100, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, -100, 100);
        ImPlot::PlotLine<float>("##data", &data[(int)begin_], end_ - begin_, 1, 0,
                                ImPlotFlags_None, 0, sizeof(float));
        ImPlot::EndPlot();
    }
}

void PlotApp::drawClock(const char* angleTitle, const char* title, const Vec2& pos, float radius,
                        float radians, bool clockwise, float offsetRadians,
                        bool useNegativeAngle, const char* vertAxisTitle,
                        const char* horzAxisTitle) {
    auto& renderer = Context::GetInst().renderer;
    font_->SetPtSize(40);
    renderer->DrawText((angleTitle + std::to_string(Rad2Deg(radians))).c_str(),
                       pos - Vec2{0, radius + 40}, *font_, Color::White);
    
    font_->SetPtSize(20);
    renderer->FillCircle({pos, radius}, Color{0.77, 0.77, 0.77}, 50);
    renderer->FillCircle({pos, radius - 4}, Color{0.93, 0.93, 0.93}, 50);
    renderer->FillCircle({pos, radius - 10}, Color{0.86, 0.86, 0.86}, 50);
    renderer->FillCircle({pos, radius - 12}, Color{0.93, 0.93, 0.93}, 50);

    int lineCount = 60;
    int boldLinePadding = 5;
    float angleStep = 2.0 * PI / lineCount;
    float outRadius = radius - 20;
    float innerRadius = radius - 30;
    for (int i = 0; i < lineCount; i++) {
        float angle = i * angleStep;
        Vec2 inner =
            pos + Vec2{cos(angle) * innerRadius, sin(angle) * innerRadius};
        Vec2 outer = pos + Vec2{cos(angle) * outRadius, sin(angle) * outRadius};
        renderer->DrawLine(inner, outer, Color::Black);

        if (i % boldLinePadding == 0 && i != lineCount - 1) {
            constexpr float offset = 10;
            Vec2 inner = pos + Vec2{cos(angle) * (innerRadius - offset),
                                    sin(angle) * (innerRadius - offset)};
            Vec2 outer = pos + Vec2{cos(angle) * (outRadius - offset),
                                    sin(angle) * (outRadius - offset)};

            constexpr float textOffset = 30;
            Vec2 textInner =
                pos + Vec2{cos((clockwise ? 1 : -1) * (angle + offsetRadians)) * (innerRadius - textOffset),
                           sin((clockwise ? 1 : -1) * (angle + offsetRadians)) * (innerRadius - textOffset)};
            renderer->DrawLineWithWidth(inner, outer, Color::Black, 3);
            int angle = 30 * i / boldLinePadding;
            int presentAngle = useNegativeAngle ? (angle > 180 ? angle - 360 : angle) : angle;
            angle = angle > 360 ? angle - 360 : angle;
            renderer->DrawText((std::to_string(presentAngle) + "°").c_str(), textInner, *font_,
                               Color::Black);

            Vec2 triP1 = Vec2{-radius * 0.2f, -10};
            Vec2 triP2 = Vec2{-radius * 0.2f, 10};
            Vec2 triP3 = Vec2{radius * 0.6f, 0};
            triP1 = pos + Rotate(triP1, (clockwise ? 1: -1) * Rad2Deg(radians + offsetRadians));
            triP2 = pos + Rotate(triP2, (clockwise ? 1: -1) * Rad2Deg(radians + offsetRadians));
            triP3 = pos + Rotate(triP3, (clockwise ? 1: -1) * Rad2Deg(radians + offsetRadians));
            renderer->FillTriangle(triP1, triP2, triP3, Color::Black);
        }
    }

    float halfLineLen = radius * 0.6f;
    Vec2 horiStart = pos - Vec2{halfLineLen, 0};
    Vec2 horiEnd = pos + Vec2{halfLineLen, 0};
    Vec2 vertStart = pos - Vec2{0, halfLineLen};
    Vec2 vertEnd = pos + Vec2{0, halfLineLen};
    renderer->DrawLineWithWidth(vertStart, vertEnd, Color::Black, 1.2);
    renderer->DrawLineWithWidth(horiStart, horiEnd, Color::Black, 1.2);
    renderer->FillTriangle(horiEnd - Vec2{0, 10}, horiEnd + Vec2{0, 10}, horiEnd + Vec2{15, 0}, Color::Black);
    renderer->FillTriangle(vertStart - Vec2{10, 0}, vertStart + Vec2{10, 0}, vertStart - Vec2{0, 15}, Color::Black);

    font_->SetPtSize(30);
    renderer->DrawText(vertAxisTitle, vertStart + Vec2{20, 0}, *font_, Color::Black);
    renderer->DrawText(horzAxisTitle, horiEnd + Vec2{0, 20}, *font_, Color::Black);
    renderer->DrawText(title, pos + Vec2{20, radius * 0.4f}, *font_, Color::Black);
}

void PlotApp::setLineColor(const ImColor& color) {
    ImPlot::SetNextLineStyle(color);
}

void PlotApp::drawBLength(float length, const char* title, const Vec2& pos) {
    auto& renderer = Context::GetInst().renderer;
    Rect rect;
    rect.size = Vec2{300, 150};
    rect.position = pos - rect.size * 0.5;
    renderer->FillRect(rect, Color::Black);
    rect.position += Vec2{5, 5};
    rect.size -= Vec2{5, 5} * 2;
    renderer->FillRect(rect, Color::White);
    renderer->DrawText((title + std::to_string(length)).c_str(), pos, *font_, Color::Black);
}