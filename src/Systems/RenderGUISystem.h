#include "../ECS/ECS.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_sdlrenderer2.h>

#pragma once

class RenderGUISystem : public System {
public:
  void Update(SDL_Renderer *renderer) {
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    //
    if (ImGui::Begin("yo yo")) {
      ImGui::Text("heheh");
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
  }
};