#include "app.hpp"

#include "context.hpp"
#include "log.hpp"

#include <filesystem>
#include <fstream>

void PlotApp::OnInit() {
    font_ = Context::GetInst().fontMgr->Load("assets/simhei.ttf", "simhei");
    font_->SetPtSize(20);
    
    auto& args = Context::GetInst().args->args;
    if (args.empty()) {
        std::cout << "you must provide data file" << std::endl;
        return;
    }
    
    std::ifstream file(args[0]);
    if (file.fail()) {
        LOGE("data.csv can't open");
        return;
    }

    while (!file.eof()) {
        char discard;
        float value;

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
    }
}

void PlotApp::OnUpdate(float deltaTime) {
    initStyle();
    increaseRange(deltaTime);

    plotOnePolyline("x-samples", xData_);
    plotOnePolyline("y-samples", yData_);
    plotOnePolyline("z-samples", zData_);

    quitStyle();

    ImPlot::ShowDemoWindow();

    static float i = 0;

    float xAngle = 0;
    float zAngle = 0;
    {
        if (end_ >= 0 && end_ < xData_.size()) {
            float x = xData_[end_];
            float y = yData_[end_];
            float z = zData_[end_];

            // Vec2 p{1, 0};
            // p = Rotate(p, i * 0.1);
            // i++;
            // float x = p.x;
            // float y = p.y;
            // float z = 1;
            float len = std::sqrt(x * x + y * y + z * z);

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
        drawClock("X平面角度", Vec2{800, 300}, 180, xAngle);
        drawClock("Z平面角度", Vec2{800, 800}, 180, zAngle);
    }
}

void PlotApp::initStyle() {
    ImPlot::PushColormap(ImPlotColormap_Deep);
    backupStyle_ = ImPlot::GetStyle();

    ImPlotStyle& style              = ImPlot::GetStyle();

    ImVec4* colors                  = style.Colors;
    colors[ImPlotCol_Line]          = ImVec4(1, 0, 0, 1);
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
        ImPlot::PlotLine<float>("data", &data[(int)begin_], end_ - begin_, 1, 0,
                                ImPlotFlags_None, 0, sizeof(float));
        ImPlot::EndPlot();
    }
}

void PlotApp::drawClock(const char* title, const Vec2& pos, float radius, float radians) {
    font_->SetPtSize(40);
    auto& renderer = Context::GetInst().renderer;
    renderer->DrawText(title, pos - Vec2{0, radius + 40}, *font_, Color::White);

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
        Vec2 inner = pos + Vec2{cos(angle) * innerRadius, sin(angle) * innerRadius};
        Vec2 outer = pos + Vec2{cos(angle) * outRadius, sin(angle) * outRadius};
        renderer->DrawLine(inner, outer, Color::Black);

        if (i % boldLinePadding == 0) {
            constexpr float offset = 10;
            Vec2 inner = pos + Vec2{cos(angle) * (innerRadius - offset),
                                    sin(angle) * (innerRadius - offset)};
            Vec2 outer = pos + Vec2{cos(angle) * (outRadius - offset),
                                    sin(angle) * (outRadius - offset)};

            constexpr float textOffset = 30;
            Vec2 textInner = pos + Vec2{cos(angle) * (innerRadius - textOffset),
                                                sin(angle) * (innerRadius - textOffset)};
            renderer->DrawLineWithWidth(inner, outer, Color::Black, 3);
            int angle = 30 * i / boldLinePadding + 90;
            angle = angle > 360 ? angle - 360 : angle;
            renderer->DrawText(std::to_string(angle).c_str(), textInner, *font_,
                               Color::Black);

            Vec2 triP1 = Vec2{-10, (radius - 100)};
            Vec2 triP2 = Vec2{10, (radius - 100)};
            Vec2 triP3 = Vec2{0, -(radius - 50)};
            triP1 = pos + Rotate(triP1, Rad2Deg(radians));
            triP2 = pos + Rotate(triP2, Rad2Deg(radians));
            triP3 = pos + Rotate(triP3, Rad2Deg(radians));
            renderer->FillTriangle(triP1, triP2, triP3, Color::Black);
        }
    }
}