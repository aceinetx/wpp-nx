#include "../submodules/imgui/imgui.h"
#include <filesystem>
#include <format>
#include <vector>

struct Program {
  std::string path;
  std::string name;
};

static std::vector<Program> program_list;

void RefreshPrograms() {
  program_list.clear();
  for (auto const &dir_entry :
       std::filesystem::directory_iterator("sdmc:/wpp")) {
    if (!dir_entry.is_regular_file())
      continue;
    if (dir_entry.path().extension().string() == ".wpp") {
      Program program;
      program.path = dir_entry.path();
      program.name = dir_entry.path().stem();
      program_list.push_back(program);
    }
  }
}

void RenderImGui() {
  ImGuiIO &io = ImGui::GetIO();

  ImGui::SetNextWindowPos({0, 0});
  ImGui::SetNextWindowSize({io.DisplaySize.x, io.DisplaySize.y});
  ImGui::Begin("Watermelon++", nullptr,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

  if (ImGui::Button("Refresh")) {
    RefreshPrograms();
  }
  ImGui::SameLine();
  ImGui::Text("Your programs should go at sdmc:/wpp");

  ImGui::Separator();

  for (Program &program : program_list) {
    ImGui::Button(program.name.c_str());
  }

  ImGui::End();
}
