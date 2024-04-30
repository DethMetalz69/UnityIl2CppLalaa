// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include "ImGui/imgui.h"

#include <functional>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

#include <Windows.h>
#include <conio.h> 
#include <cstdlib>
#include "UnityResolve.hpp"
#include "UnityBlaBla.h"

#include "Windows/Window.h"
#include "Windows/HierarchyWindow.h"
#include "Windows/InspectorWindow.h"


typedef UnityResolve::UnityType::GameObject GameObject;
typedef UnityResolve::UnityType::Transform Transform;
typedef UnityResolve::UnityType::UnityObject UnityObject;

void Loadimgui() {
  Window::InitGL();

  auto hierarchyWindow = new HierarchyWindow(true);
  Window::windows.push_back((Window*)hierarchyWindow);

  //auto iwi1 = new InspectorWindow();
  //iwi1->m_sObjName = "asbs";
  //auto iwi2 = new InspectorWindow();
  //iwi2->m_sObjName = "1233";

  //Window::windows.push_back((Window*)iwi1);
  //Window::windows.push_back((Window*)iwi2);

  while (Window::windows.size() > 0) {
    //printf("main loop\n");

    auto t1 = std::chrono::system_clock::now();

    //UnityResolve::ThreadAttach();

    UnityBlaBla::UpdateAllObjects();
    if (false == Window::SMainLoop()) {
      break;
    }

    //UnityResolve::ThreadDetach();
    
    auto t2 = std::chrono::system_clock::now();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

    int timeToSleepFrame = 16 - millis;
    if (timeToSleepFrame > 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(timeToSleepFrame));
    }
  }


  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwTerminate();
}

/*
* 
* 
#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_options.hpp"
#include "ftxui/component/screen_interactive.hpp" 
#include "ftxui/component/receiver.hpp"
using namespace ftxui;


MenuEntryOption HierarchyItem(ftxui::Color c) {
  MenuEntryOption option;
  option.animated_colors.foreground.enabled = true;
  option.animated_colors.background.enabled = true;
  option.animated_colors.background.active = c;
  option.animated_colors.background.inactive = Color::Black;
  //option.animated_colors.foreground.active = Color::White;
  //option.animated_colors.foreground.inactive = c;
  return option;
}

void Scene_GetRootGameObjects() {
  auto screen = ScreenInteractive::Fullscreen();

  int selected = 1;
  auto me1 = MenuEntry(" 1. improve", HierarchyItem(Color::Cyan));
  auto me2 = MenuEntry(" 1. tolerant", HierarchyItem(Color::Cyan));

  std::vector<bool> states = {false, true};
  bool b1 = false;
  bool b2 = false;

  auto menu = Container::Vertical(
    {
        me1,
        me2,
        MenuEntry(" 3. career"),
        MenuEntry(" 4. cast"),
        MenuEntry(" 5. question"),
        Checkbox("Checkbox" + std::to_string(0), &b1),
        Checkbox("Checkbox" + std::to_string(1), &b2),
    },
    &selected);

  menu->SetActiveChild(me2);


  bool hover = false;

  auto content = Renderer([&] { return text("Content") | center | center; });
  auto hierarchy = Renderer([&] { return vbox({ text("Hierarchy"), separator(), menu->Render() | flex }); });
  auto inspector = Renderer([&] { return vbox({ text("Inspector"), separator(), hbox({}) | flex }); });

  int hierarchy_size = 30;
  int inspector_size = 30;

  hierarchy |= Hoverable([&](bool h) { hover = h; });

  auto container = content;
  container = ResizableSplitLeft(inspector, container, &inspector_size);
  container = ResizableSplitLeft(hierarchy, container, &hierarchy_size);

  container->SetActiveChild(hierarchy);

  auto renderer = Renderer(container, [&] {
    //return ; 
    return vbox({
      // -------- Top panel --------------
      hbox({ text("menu") | center }),
      // -------- Centext --------------
      separator(),
      hbox({
        container->Render()
        }) | flex,
      separator(),
      // -------- Bottom panel --------------
      hbox({ text("logs") | center }),
      }) |
      borderLight;
  });

  renderer |= CatchEvent([&](Event event) {
    if (event == Event::Character('q')) {
      screen.ExitLoopClosure()();
      return true;
    }
    return false;
    });
  
  screen.Loop(renderer);

  //std::cout << "Selected element = " << selected << std::endl;
  
  std::cout << "Scene Count " << UnityResolve::UnityType::SceneManager::GetSceneCount() << std::endl;

  auto camera = UnityResolve::UnityType::Camera::GetMain();
  std::cout << "Camera Depth " << camera->GetDepth() << std::endl;

  
  auto allObjects = UnityObject::FindObjectsOfType<GameObject>();
  std::vector<GameObject*> rootObjects{};

  for (auto obj : allObjects) {
    auto root = obj->GetTransform()->GetRoot()->GetGameObject();
    bool found = false;
    for (auto robj : rootObjects) {
      if (root == robj) {
        found = true;
        break;
      }
    }

    if (found == false) {
      rootObjects.push_back(root);
      std::cout << "obj " << root->GetName() << std::endl;
    }
  }
  
}
*/
uint32_t __stdcall Attached(HMODULE hModule) {
  HWND consoleWnd = GetConsoleWindow();
  if (!consoleWnd) {
    AllocConsole();
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
  }
  
  std::cout << "Attached" << std::endl;

  UnityResolve::Init(GetModuleHandle(L"GameAssembly.dll"), UnityResolve::Mode::Il2Cpp);

  UnityResolve::ThreadAttach();

  std::cout << "Scene Count " << UnityResolve::UnityType::SceneManager::GetSceneCount() << std::endl;

  auto camera = UnityResolve::UnityType::Camera::GetMain();
  std::cout << "Camera Depth " << camera->GetDepth() << std::endl;

  //UnityResolve::ThreadDetach();

  //Scene_GetRootGameObjects();
  //UnityResolve::DumpToFile("C:/Users/Administrator/Documents/RealmOfTheMadGod/Production/output/");

  

  Loadimgui();


  FreeLibraryAndExitThread(hModule, 0);
  return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
      CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Attached, hModule, 0, 0);
      break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

