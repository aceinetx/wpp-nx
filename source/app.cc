#include "app.hh"
#include "../submodules/imgui/imgui.h"
extern "C" {
#include "../wpp-oss/exec.h"
#include "../wpp-oss/lexer.h"
}
#include <cstdio>
#include <filesystem>
#include <format>
#include <fstream>
#include <thread>
#include <vector>

extern "C" {
char *wpp_input_handler();
}

struct Program {
  std::string path;
  std::string name;
};

static std::vector<Program> program_list;
static std::string text;
static Program current_program;
static wppExec *exec;
static wppLexer *lexer;
static std::thread exec_thread;
std::string output;

static bool running_program = false;
static bool done_executing = false;

static std::string readFile(const std::string &filename) {
  std::ifstream file(filename);
  if (!file) {
    return "";
  }

  std::stringstream buffer;
  buffer << file.rdbuf();

  return buffer.str();
}

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

void *ExecThread() {
  lexer = wpp_lexer_new(strdup((char *)readFile(current_program.path).c_str()));
  exec = wpp_exec_new(lexer);

  wpp_exec_run(exec);
  if (*exec->error) {
    output += "wpp: " + std::string(exec->error) + "\n";
  } else {
    output += "wpp: program exited with code " +
              std::to_string(exec->exit_level) + "\n";
  }

  done_executing = true;

  free(lexer->code);

  wpp_lexer_free(lexer);
  wpp_exec_free(exec);

  output += "wpp: done\n";
  ImGui::SetScrollHereY(1.0f);

  return nullptr;
}

void RunProgram(Program program) {
  current_program = program;
  running_program = true;
  done_executing = false;
  output.clear();

  output += "wpp: Running program " + current_program.path + "\n";

  ExecThread();
}

void RenderImGuiProgram() {
  if (!running_program) {
    RenderImGui();
    return;
  }

  ImGuiIO &io = ImGui::GetIO();

  ImGui::SetNextWindowPos({0, 0});
  ImGui::SetNextWindowSize({io.DisplaySize.x, io.DisplaySize.y});
  ImGui::Begin("Watermelon++", nullptr,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

  ImGui::Text("%s", output.c_str());
  if (done_executing) {
    if (ImGui::Button("Back to program explorer")) {
      running_program = false;
    }
  }

  ImGui::End();
}

void RenderImGui() {
  if (running_program) {
    RenderImGuiProgram();
    return;
  }

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
    if (ImGui::Button(program.name.c_str())) {
      RunProgram(program);
    }
  }

  ImGui::End();
}
