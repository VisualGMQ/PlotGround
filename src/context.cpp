#include "context.hpp"
#include "flags.hpp"
#include "log.hpp"
#include "math.hpp"
#include "renderer.hpp"

Context* Context::inst = nullptr;

void Context::Init(int argc, char** argv) {
    inst = new Context;
    inst->postInit(argc, argv);
}

void Context::Destroy() {
    delete inst;
}

void Context::postInit(int argc, char** argv) {
    initSDL();

    window = std::make_unique<Window>("Treasure Looter", 1024, 1024);
    renderer = std::make_unique<Renderer>(*window);
    if (!window || !renderer) {
        quitSDL();
        exit(1);
    }
    initImGui();

    keyboard = std::make_unique<Keyboard>();
    mouse = std::make_unique<Mouse>();
    time = std::make_unique<Time>();
    fontMgr = std::make_unique<FontManager>();
    args = std::make_unique<Args>();

    for (int i = 1; i < argc; i++) {
        args->args.push_back(argv[i]);
    }
    
    initApp();
}

void Context::initSDL() {
    SDL_Init(SDL_INIT_EVERYTHING);
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif
    IMG_Init(IMG_INIT_PNG);
    Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG);
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) != 0) {
        LOGE("open audio device failed");
    }
    TTF_Init();

    SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" );

#ifdef TL_ANDROID
    SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");
#endif
}

void Context::quitSDL() {
    TTF_Quit();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}

Context::~Context() {
    quitImGui();

    if (app) {
        app->OnQuit();
    }
    args.reset();
    app.reset();
    time.reset();
    mouse.reset();
    keyboard.reset();
    renderer.reset();
    fontMgr.reset();
    window.reset();

    quitSDL();
}

void Context::Update() {
    time->BeginRecordElapse();
    
    handleEvent();

    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    renderer->Clear(Color{0.3, 0.3, 0.3});

    app->OnUpdate(time->GetElapse() / 1000.0f);
    keyboard->Update();
    mouse->Update();

    renderer->Update();

    ImGui::Render();
    ImGuiIO& io = ImGui::GetIO();
    renderer->SetScale(
        {io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y});
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(),
                                          renderer->renderer_);

    renderer->Present();

    time->EndRecordElapse();
}

void Context::handleEvent() {
    while (SDL_PollEvent(&event_)) {
        ImGui_ImplSDL2_ProcessEvent(&event_);
        if (event_.type == SDL_QUIT) {
            shouldExit_ = true;
        }
        keyboard->HandleEvent(event_);
        mouse->HandleEvent(event_);
    }
}

void Context::initApp() {
    app = std::make_unique<PlotApp>();
    app->OnInit();
}

void Context::initImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // Enable Docking

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForSDLRenderer(window->window_, renderer->renderer_);
    ImGui_ImplSDLRenderer2_Init(renderer->renderer_);

    ImPlot::CreateContext();
}

void Context::quitImGui() {
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
}