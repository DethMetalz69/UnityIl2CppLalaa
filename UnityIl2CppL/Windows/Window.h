#pragma once

#include <string>
#include <thread>
#include <functional>
#include <vector>
#include <mutex>

#include "../ImGui/imgui.h"
#include "../ImGui/backends/imgui_impl_glfw.h"
#include "../ImGui/backends/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>

class Window {

private:
  void renderMainWindow();

protected:
  int* width = new int(0);
  int* height = new int(0);
  float fWidth;
  float fHeight;
  bool m_main;

  bool windowOpen = true;

  std::thread mainLoopTask;

  std::vector<std::function<void()>> mainWindowFunctions{};

  ImGuiContext * ctx;
  ImGuiContext * ctx2;

public:
  Window(const std::string &title, bool main = false);
  ~Window();

  void Close();

  bool MainLoop();
  static bool SMainLoop();


  static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
  static void InitGL();

  static std::vector<Window*> windows;

  template<typename T>
  T * NewWindow();

  static GLFWwindow* m_pWindow;
};

