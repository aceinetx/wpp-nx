#include <cstdlib>

#include "app.hh"
#include <filesystem>
#include <imgui.h>
#include <switch.h>

#include "imgui_impl_deko3d.h"
#include "util.h"

extern "C" void userAppInit() {
  plInitialize(PlServiceType_User);
  romfsInit();
#ifdef DEBUG
  socketInitializeDefault();
  nxlinkStdio();
#endif
}

extern "C" void userAppExit() {
  plExit();
  romfsExit();
#ifdef DEBUG
  socketExit();
#endif
}

int main(int argc, char *argv[]) {
  std::filesystem::create_directory("sdmc:/wpp");
  RefreshPrograms();

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();
  // ImGuiIO &io = ImGui::GetIO(); // to configure imgui

  ImGui_ImplDeko3d_Init();

  while (appletMainLoop()) {
    u64 down = ImGui_ImplDeko3d_UpdatePad();
    if (down & HidNpadButton_Plus) // "+" to exit
      break;

    ImGui_ImplDeko3d_NewFrame();
    ImGui::NewFrame();

    RenderImGui();

    ImGui::Render();
    ImGui_ImplDeko3d_RenderDrawData(ImGui::GetDrawData());
  }

  ImGui_ImplDeko3d_Shutdown();
  ImGui::DestroyContext();
  return 0;
}
