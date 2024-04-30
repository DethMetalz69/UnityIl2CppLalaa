#include "Window.h"

#include <stdio.h>
#include <functional>

#include "HierarchyWindow.h"
#include "InspectorWindow.h"


std::vector<Window*> Window::windows{};
GLFWwindow* Window::m_pWindow = nullptr;

static void glfw_error_callback(int error, const char* description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}


bool Window::MainLoop() {
  renderMainWindow();

  return true;

}

bool Window::SMainLoop() {
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  if (!glfwWindowShouldClose(m_pWindow)) {
    //ImGui::SetCurrentContext(this->ctx);

    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    glfwPollEvents();


    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    size_t windowsSize = windows.size();
    //for (auto w : windows) {
    for (size_t i = 0; i < windowsSize; i++) {
      windows[i]->MainLoop();

      if (false == windows[i]->windowOpen) {
        auto win = windows[i];
        windows.erase(windows.begin() + i);
        delete win;
        break;
      }
    }

    // Rendering
    ImGui::Render();

    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();

    int display_w, display_h;
    glfwGetFramebufferSize(m_pWindow, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    //glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    //glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(m_pWindow);


    return true;
  }

  return false;
}

Window::Window(const std::string &title, bool main) {
  this->m_main = main;


  this->ctx = ImGui::GetCurrentContext();

  /*
  if (false == main) {
    this->ctx = ImGui::CreateContext();
    ImGui::SetCurrentContext(this->ctx);
  }
  else {
    this->ctx = ImGui::GetCurrentContext();
  }
  */
}

Window::~Window() {
  //ImGui::SetCurrentContext(this->ctx);

  // Cleanup
  //ImGui_ImplOpenGL3_Shutdown();
  //ImGui_ImplGlfw_Shutdown();
  //ImGui::DestroyContext();

  //glfwDestroyWindow(m_pWindow);
}

void Window::Close() {
  //glfwDestroyWindow(m_pWindow);
}

void Window::renderMainWindow() {
  //glfwGetWindowSize(m_pWindow, width, height);
  //fWidth = *width + 0.f;
  //fHeight = *height + 0.f;

  for (auto fun : mainWindowFunctions) { fun(); }
}

void Window::InitGL() {
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
    return;

  // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
  const char* glsl_version = "#version 100";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
  const char* glsl_version = "#version 150";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
  const char* glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

  // Create window with graphics context
  m_pWindow = glfwCreateWindow(400, 600, "Something", nullptr, nullptr);
  if (m_pWindow == nullptr)
    return;

  glfwMakeContextCurrent(m_pWindow);
  glfwSwapInterval(1); // Enable vsync

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  auto vtx = ImGui::CreateContext();
  ImGui::SetCurrentContext(vtx);

  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(m_pWindow, true);
#ifdef __EMSCRIPTEN__
  ImGui_ImplGlfw_InstallEmscriptenCanvasResizeCallback("#canvas");
#endif
  ImGui_ImplOpenGL3_Init("#version 300 es");

  glfwSetFramebufferSizeCallback(m_pWindow, framebuffer_size_callback);

}

template HierarchyWindow* Window::NewWindow<HierarchyWindow>();
template InspectorWindow* Window::NewWindow<InspectorWindow>();

template<typename T>
T * Window::NewWindow() {
  auto win = new T();
  Window::windows.push_back((Window*)win);

  //ImGui::SetCurrentContext(this->ctx);

  return win;
}

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}
