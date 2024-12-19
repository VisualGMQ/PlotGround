#include "app.hpp"

#include "context.hpp"
#include "log.hpp"

#include <filesystem>
#include <fstream>

void PlotApp::OnInit() {
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

    Context::GetInst().renderer->DrawLineWithWidth(
        Vec2{100, 100}, Vec2{200, 200}, Color::Red, 5);
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

void PlotApp::plotOnePolyline(const char* title, const std::vector<float>& data) {
    if (data.empty()) {
        return;
    }
    
    if (ImPlot::BeginPlot(title)) {
        ImPlot::SetupAxisLimits(ImAxis_X1,0, 100, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, -100, 100);
        ImPlot::PlotLine<float>("data", &data[(int)begin_], end_ - begin_, 1,
                                0, ImPlotFlags_None, 0,
                                sizeof(float));
        ImPlot::EndPlot();
    }
        
}