#pragma once

class App {
public:
    virtual void OnInit() = 0;
    virtual void OnQuit() = 0;
    virtual void OnUpdate(float deltaTime) = 0;
};

class PlotApp: public App {
public:
    void OnInit();
    void OnQuit() {}
    void OnUpdate(float deltaTime);

private:
    ImPlotStyle backupStyle_;
    std::vector<float> xData_;
    std::vector<float> yData_;
    std::vector<float> zData_;
    float speed_ = 10.0f;
    size_t count_ = 100;
    float begin_  = 0;
    float end_ = 0;
    
    void initStyle();
    void quitStyle();
    void increaseRange(float deltaTime);
    void plotOnePolyline(const char* title, const std::vector<float>& data);
};
