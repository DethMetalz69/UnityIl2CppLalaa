#include "HierarchyWindow.h"

#include "InspectorWindow.h"
#include "../UnityBlaBla.h"



typedef UnityResolve::UnityType::GameObject GameObject;
typedef UnityResolve::UnityType::Transform Transform;
typedef UnityResolve::UnityType::UnityObject UnityObject;
typedef UnityResolve::UnityType::Component Component;


ImGuiTreeNodeFlags GetTreeNodeFlags(Transform* t) {
  ImGuiTreeNodeFlags flags{};
  if (t->GetChildCount() == 0) {
    flags |= ImGuiTreeNodeFlags_Leaf;
  }

  return flags;
}

void HierarchyWindow::BuildHierarchy(Transform* t) {

  auto childCount = t->GetChildCount();
  for (int i = 0; i < childCount; i++) {
    auto child = t->GetChild(i);

    auto iId = child->GetInstanceID();
    auto siId = std::to_string(iId);

    std::string objName = child->GetName();

    if (false == t->GetGameObject()->GetActiveInHierarchy()) {
      ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
    }
    else {
      ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(250, 250, 250, 255));
    }

    if (ImGui::TreeNodeEx(siId.c_str(), GetTreeNodeFlags(child), objName.c_str())) {
      if (ImGui::IsItemClicked()) {
        std::cout << "inspect " << objName << std::endl;

        // open inspector window
        auto inspectorWindow = Window::NewWindow<InspectorWindow>();
        inspectorWindow->m_pTransform = child;
        inspectorWindow->m_sObjName = objName;
        inspectorWindow->m_instanceId = iId;
      }

      BuildHierarchy(child);
      ImGui::TreePop();
    }

    ImGui::PopStyleColor();
  }
}

void HierarchyWindow::FirstCall() {
  //ImGui::SetWindowPos({ 0, 0 });
  ImGui::SetWindowSize({ fWidth, fHeight });
}

void HierarchyWindow::MainLoop() {

  ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar /* | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar*/;

  if (ImGui::Begin("Hierarchy", &windowOpen, windowFlags)) {

    if (firstCall == false) {
      firstCall = true;
      FirstCall();
    }
    
    for (auto obj : UnityBlaBla::m_rootGameObjects) {
      auto t = obj->GetTransform();
      std::string objName = obj->GetName();

      if (false == obj->GetActiveInHierarchy()) {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
      }
      else {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(250, 250, 250, 255));
      }

      if (ImGui::TreeNodeEx(std::to_string(obj->GetInstanceID()).c_str(), GetTreeNodeFlags(t), objName.c_str())) {

        if (ImGui::IsItemClicked()) {
          std::cout << "inspect " << objName << std::endl;

          // open inspector window
          auto inspectorWindow = Window::NewWindow<InspectorWindow>();
          inspectorWindow->m_pTransform = t;
          inspectorWindow->m_sObjName = objName;
        }

        BuildHierarchy(t);
        ImGui::TreePop();
      }

      ImGui::PopStyleColor();
    }
  }
  ImGui::End();

}

HierarchyWindow::HierarchyWindow(bool main) : Window("Hierarchy", main) {
  fWidth = 400;
  fHeight = 600;
  mainWindowFunctions.push_back(std::bind(&HierarchyWindow::MainLoop, this));
}
