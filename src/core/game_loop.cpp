#include <cstddef>
#include <SDL3/SDL.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <iostream>
#include <fstream>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

// Helps one to find out if a button was recently pressed
// One of these structs needs to be allocated for each button tracked thus
struct ButtonTimer {
	static constexpr char ACTIVE{0x01};
	static constexpr char RECENTLYPRESSED{0x02};

	float recently {0.0f};
	float timeSincePressed {0.0f};
	char flags {};

	ButtonTimer() = default;
	explicit ButtonTimer(float g_recently = 0.1f)
		: recently {g_recently}
	{}

	void Press () {
		timeSincePressed = 0.0f;
		flags = ACTIVE | RECENTLYPRESSED;
	}
	// Needs to be called at the start of the update loop
	void Update(float deltaTime) {
		if ((flags & ACTIVE) && (timeSincePressed < recently)) {
			timeSincePressed += deltaTime;
		}
		else {
			flags = 0;
		}
	}
};

struct AppData {
	static constexpr int width {1280};
	static constexpr int height {720};

	SDL_Renderer *renderer{nullptr};
	SDL_Window *window {nullptr};

	int lastTime {0};
	float deltaTime {0.0f};

};

static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void InitImgui(SDL_Renderer* renderer, SDL_Window* window) {
    // Basic setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Set appearance and scaling of ImGui
    //ImGuiStyle& style = ImGui::GetStyle();
    //style.ScaleAllSizes(GameSettings::INITIALSCALE);
    // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    //style.FontScaleDpi = GameSettings::INITIALSCALE;
    ImGui::StyleColorsDark();

    // Enable keyboard navigation in imgui
    ImGuiIO& io {ImGui::GetIO()};
    io.FontGlobalScale = 2;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Initialise ImGui globals for SDL
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);
}

void DrawImgui(SDL_Renderer* renderer) {
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    ImGuiIO& io {ImGui::GetIO()};
    ImGui::Begin("Tab");
    ImGui::End();

    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
}

void QuitImgui(){
    // Shutdown ImGui
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}


SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    std::cout << "App initialised\n";
    if(!SDL_InitSubSystem(SDL_INIT_VIDEO)){
        SDL_Log("Failed to init video: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

	AppData* appData {new AppData()};

    SDL_SetHint("SDL_RENDER_VSYNC", "1");
    appData->window = SDL_CreateWindow("Low Level Game", appData->width, appData->height, SDL_WINDOW_RESIZABLE);
    if(!appData->window) {
        SDL_Log("Failed to initialise window: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    appData->renderer = SDL_CreateRenderer(appData->window, NULL);

    if (!appData->renderer){
    	SDL_DestroyWindow(appData->window);
        SDL_Log("Failed to initialised renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetRenderVSync(appData->renderer, 1);

    //SDL_SetRenderLogicalPresentation(appData->renderer, GAMEWIDTH, GAMEHEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    SDL_ShowWindow(appData->window);

    SDL_RaiseWindow(appData->window);

    InitImgui(appData->renderer, appData->window);

	*appstate = appData;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event){
	AppData* appData {(AppData*)appstate};

    ImGui_ImplSDL3_ProcessEvent(event);
    switch(event->type) {
        case (SDL_EVENT_QUIT) : {
            return SDL_APP_SUCCESS;
        } break;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate){
	AppData* appData {(AppData*)appstate};
    SDL_SetRenderDrawColor(appData->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(appData->renderer);
    // Get DeltaTime
    Uint64 currentTime {SDL_GetPerformanceCounter()};
    float deltaTime {(currentTime - appData->lastTime) / (float)SDL_GetPerformanceFrequency()};
    appData->lastTime = currentTime;

	// Update button timers here if any
	// Update

    DrawImgui(appData->renderer);

    SDL_RenderPresent(appData->renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result){
	AppData* appData {(AppData*)appstate};
    SDL_DestroyRenderer(appData->renderer);
    SDL_DestroyWindow(appData->window);

    QuitImgui();
}