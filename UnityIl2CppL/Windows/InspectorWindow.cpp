#include "InspectorWindow.h"

#include "../UnityResolve.hpp"
#include "../UnityBlaBla.h"

#include <iostream>
#include <iomanip>
#include <sstream>

#include <absl/strings/str_split.h>
#include <absl/strings/str_replace.h>


typedef UnityResolve::UnityType::GameObject GameObject;
typedef UnityResolve::UnityType::Transform Transform;
typedef UnityResolve::UnityType::UnityObject UnityObject;
typedef UnityResolve::UnityType::Component Component;


#define INSPECTOR_FIELD(str, val)                                              \
  ImGui::Columns(2);                                                           \
  ImGui::Text(str.c_str());                                                    \
  ImGui::NextColumn();                                                         \
  ImGui::Text(val.c_str());                                                    \

#define INSPECTOR_FIELD_VEC(str, vec, in)                                      \
  ImGui::Columns(2);                                                           \
  ImGui::Text(str.c_str());                                                    \
  ImGui::NextColumn();                                                         \
  ImGui::in(("##" + str).c_str(), vec);                                        \
  delete [] vec;                                                               \


void AddUnderLine(ImColor col_) {
  ImVec2 min = ImGui::GetItemRectMin();
  ImVec2 max = ImGui::GetItemRectMax();
  min.y = max.y;
  ImGui::GetWindowDrawList()->AddLine(min, max, col_, 1.0f);
}

void TextCentered(std::string text) {
  auto windowWidth = ImGui::GetWindowSize().x;
  auto textWidth = ImGui::CalcTextSize(text.c_str()).x;

  ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
  ImGui::Text(text.c_str());
}

std::string BuildSLinkLocation(Transform * t) {
  if (t == nullptr) {
    return "*nullptr*";
  }

  std::string res = t->GetName();
  auto parent = t->GetParent();

  while (nullptr != parent) {
    res.insert(0, parent->GetName() + " > ");

    // some start to loop
    if (parent == parent->GetParent()) {
      break;
    }

    parent = parent->GetParent();
  }

  return res;
}

std::string ulonglong_to_hex(void * value) {
  std::stringstream ss;
  ss << std::hex << value;
  return ss.str();
}

std::tuple<UnityResolve::Assembly*, UnityResolve::Class*> GetAssemblyAndClassFromType(UnityResolve::Type * type) {

  std::vector<std::string> asmSplits = absl::StrSplit(type->assembly_qualified_name, ", ");
  auto pa = UnityResolve::Get(asmSplits[1]+".dll");

  //printf("size %d, pa 1 %s, %llx, q %s\n", asmSplits.size(), asmSplits[1].c_str(), pa, type->assembly_qualified_name.c_str());

  std::vector<std::string> classNameSplits = absl::StrSplit(type->name, ".");

  std::string className = classNameSplits.back();
  auto pc = pa->Get(absl::StrReplaceAll(className, { {"[]", ""} }));

  return std::make_tuple(pa, pc);
}

bool CheckIsValueTypeOf(UnityResolve::Class * klass, int maxDepth = 5) {
  if (klass->parent.empty() || maxDepth <= 0) {
    return false;
  }

  if (klass->parent == "ValueType") {
    return true;
  }

  if (klass->parent == "Object") {
    return false;
  }

  // find first type
  for (const auto& pAssembly : UnityResolve::assembly) {
    for (const auto& pClass : pAssembly->classes) {
      if (pClass->name == klass->parent) {
        return CheckIsValueTypeOf(pClass, maxDepth -1);
      }
    }
  }

  return false;
}

bool InspectorWindow::BuildField(UnityResolve::Class* klass, UnityResolve::Assembly* pAssembly, void * c, std::string title) {
  if (nullptr == klass || nullptr == pAssembly || c == nullptr) {
    return false;
  }

  ImGui::Columns(1);
  if (ImGui::TreeNode(title.c_str())) {
    //printf("%s is open\n", title.c_str());

    
    auto pClass = klass;

    while (nullptr != pClass && pClass->name != "ValueType") {

      ImGui::Columns(2);
      ImGui::Text("# Script");
      ImGui::NextColumn();
      ImGui::Text(pClass->name.c_str());
      ImGui::NextColumn();
      ImGui::Text("# Address");
      ImVec2 min = ImGui::GetItemRectMin();
      ImGui::NextColumn();


      if (this->m_fieldComponentAddresses.find(pClass->address) == this->m_fieldComponentAddresses.end()) {
        this->m_fieldComponentAddresses[pClass->address] = ulonglong_to_hex(pClass->address);
      }

      ImGui::InputText("##Address", (char *)this->m_fieldComponentAddresses[pClass->address].c_str(), this->m_fieldComponentAddresses[pClass->address].size(), ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_AllowTabInput);
      ImVec2 max = ImGui::GetItemRectMax();

      if (ImGui::BeginPopupContextItem("Context Menu")) {
        if (ImGui::Selectable("Copy Address")) {
          ImGui::SetClipboardText(this->m_fieldComponentAddresses[pClass->address].c_str());
        }
        ImGui::EndPopup();
      }

      ImGui::Columns(1);

      max.y += 4;
      min.y = max.y;

      ImGui::Dummy(ImVec2(0.0f, 4.0f));
      ImGui::GetWindowDrawList()->AddLine(min, max, ImColor::HSV(0.8f, 0.8f, 0.8f, 1.0f), 1.0f);
      ImGui::Dummy(ImVec2(0.0f, 4.0f));

      TextCentered("Properties");
      AddUnderLine(ImColor::HSV(0.8f, 0.8f, 0.8f, 1.0f));

      // check properties
      for (const auto& pMethod : pClass->methods) {
        if (pMethod->static_function == false && pMethod->name.rfind("get_", 0) == 0 && pMethod->return_type->name != "System.Void" && pMethod->args.size() == 0) {
          //printf("property get method %s\n", pMethod->name.c_str());
          auto method = UnityResolve::Get(pAssembly->name)->Get(pClass->name)->Get<UnityResolve::Method>(pMethod->name);
          //printf("property get value for %s | %s\n", pMethod->return_type->name.c_str(), pMethod->name.c_str());

          if (pMethod->return_type->name == "System.Byte") {
            INSPECTOR_FIELD(pMethod->name.substr(4), std::to_string(method->Invoke<uint8_t>(c)));
          }
          else if (pMethod->return_type->name == "System.Boolean") {
            INSPECTOR_FIELD(pMethod->name.substr(4), std::string(method->Invoke<bool>(c) ? "true" : "false"));
          }
          else if (pMethod->return_type->name == "System.Int32") {
            INSPECTOR_FIELD(pMethod->name.substr(4), std::to_string(method->Invoke<int32_t>(c)));
          }
          else if (pMethod->return_type->name == "System.UInt32") {
            INSPECTOR_FIELD(pMethod->name.substr(4), std::to_string(method->Invoke<uint32_t>(c)));
          }
          else if (pMethod->return_type->name == "System.Int64") {
            INSPECTOR_FIELD(pMethod->name.substr(4), std::to_string(method->Invoke<int64_t>(c)));
          }
          else if (pMethod->return_type->name == "System.UInt64") {
            INSPECTOR_FIELD(pMethod->name.substr(4), std::to_string(method->Invoke<uint64_t>(c)));
          }
          else if (pMethod->return_type->name == "UnityEngine.Vector2") {
            auto vec = method->Invoke<UnityResolve::UnityType::Vector2>(c);
            auto aVec = new float[2] {vec.x, vec.y};
            INSPECTOR_FIELD_VEC(pMethod->name.substr(4), aVec, InputFloat2);
          }
          else if (pMethod->return_type->name == "UnityEngine.Vector3") {
            auto vec = method->Invoke<UnityResolve::UnityType::Vector3>(c);
            auto aVec = new float[3] {vec.x, vec.y, vec.z};
            INSPECTOR_FIELD_VEC(pMethod->name.substr(4), aVec, InputFloat3);
          }
          else if (pMethod->return_type->name == "UnityEngine.Vector4") {
            auto vec = method->Invoke<UnityResolve::UnityType::Vector4>(c);
            auto aVec = new float[4] {vec.x, vec.y, vec.z, vec.w};
            INSPECTOR_FIELD_VEC(pMethod->name.substr(4), aVec, InputFloat4);
          }
          else if (pMethod->return_type->name == "UnityEngine.Quaternion") {
            auto vec = method->Invoke<UnityResolve::UnityType::Quaternion>(c);
            auto aVec = new float[4] {vec.x, vec.y, vec.z, vec.w};
            INSPECTOR_FIELD_VEC(pMethod->name.substr(4), aVec, InputFloat4);
          }
          else if (pMethod->return_type->name == "UnityEngine.Transform") {
            auto t = method->Invoke<UnityResolve::UnityType::Transform*>(c);
            INSPECTOR_FIELD(pMethod->name.substr(4), std::string("T[ " + BuildSLinkLocation(t) + " ]"));
          }
          else if (pMethod->return_type->name == "UnityEngine.GameObject") {
            auto go = method->Invoke<UnityResolve::UnityType::GameObject*>(c);
            INSPECTOR_FIELD(pMethod->name.substr(4), std::string("G[ " + BuildSLinkLocation(go->GetTransform()) + " ]"));
          }
          else {
            auto [pa, pc] = GetAssemblyAndClassFromType(pMethod->return_type);

            if (nullptr != pc && CheckIsValueTypeOf(pc) == false) {
              auto inst = method->Invoke<void*>(c);
              //auto goTitle = title + ":" + pMethod->name.substr(4);

              if (BuildField(pc, pa, inst, pMethod->name.substr(4)) == false) {
                INSPECTOR_FIELD(pMethod->name.substr(4), std::string("[ *Empty* ]"));
              }
            }
            else {
              INSPECTOR_FIELD(pMethod->name.substr(4), std::string("[ *Undefined* ]"));
            }
          }
        }

        ImGui::Columns(1);
      }

      ImGui::Columns(1);
      TextCentered("Fields");
      AddUnderLine(ImColor::HSV(0.8f, 0.8f, 0.8f, 1.0f));

      // and check for fields
      for (const auto& pField : pClass->fields) {
        if (false == pField->static_field) {
          //printf("field get value for %s | %s\n", pField->type->name.c_str(), pField->name.c_str());

          if (pField->type->name == "System.Byte") {
            auto sVal = std::to_string(pClass->GetValue<uint8_t>(c, pField->name));
            INSPECTOR_FIELD(pField->name, sVal);
          }
          else if (pField->type->name == "System.Boolean") {
            auto sVal = std::string(pClass->GetValue<bool>(c, pField->name) ? "true" : "false");
            INSPECTOR_FIELD(pField->name, sVal);
          }
          else if (pField->type->name == "System.Int32") {
            auto sVal = std::to_string(pClass->GetValue<int32_t>(c, pField->name));
            INSPECTOR_FIELD(pField->name, sVal);
          }
          else if (pField->type->name == "System.UInt32") {
            auto sVal = std::to_string(pClass->GetValue<uint32_t>(c, pField->name));
            INSPECTOR_FIELD(pField->name, sVal);
          }
          else if (pField->type->name == "System.Int64") {
            auto sVal = std::to_string(pClass->GetValue<int64_t>(c, pField->name));
            INSPECTOR_FIELD(pField->name, sVal);
          }
          else if (pField->type->name == "System.UInt64") {
            auto sVal = std::to_string(pClass->GetValue<uint64_t>(c, pField->name));
            INSPECTOR_FIELD(pField->name, sVal);
          }
          else if (pField->type->name == "System.Single") {
            auto sVal = std::to_string(pClass->GetValue<float>(c, pField->name));
            INSPECTOR_FIELD(pField->name, sVal);
          }
          else if (pField->type->name == "System.Double") {
            auto sVal = std::to_string(pClass->GetValue<double>(c, pField->name));
            INSPECTOR_FIELD(pField->name, sVal);
          }
          else if (pField->type->name == "System.String") {
            auto sVal = pClass->GetValue<UnityResolve::UnityType::String>(c, pField->name).ToString();
            INSPECTOR_FIELD(pField->name, sVal);
          }
          else if (pField->type->name == "UnityEngine.Vector2") {
            auto vec = pClass->GetValue<UnityResolve::UnityType::Vector2>(c, pField->name);
            auto aVec = new float[2] {vec.x, vec.y};
            INSPECTOR_FIELD_VEC(pField->name, aVec, InputFloat2);
          }
          else if (pField->type->name == "UnityEngine.Vector3") {
            auto vec = pClass->GetValue<UnityResolve::UnityType::Vector3>(c, pField->name);
            auto aVec = new float[3] {vec.x, vec.y, vec.z};
            INSPECTOR_FIELD_VEC(pField->name, aVec, InputFloat3);
          }
          else if (pField->type->name == "UnityEngine.Vector4") {
            auto vec = pClass->GetValue<UnityResolve::UnityType::Vector4>(c, pField->name);
            auto aVec = new float[4] {vec.x, vec.y, vec.z, vec.w};
            INSPECTOR_FIELD_VEC(pField->name, aVec, InputFloat4);
          }
          else if (pField->type->name == "UnityEngine.Quaternion") {
            auto vec = pClass->GetValue<UnityResolve::UnityType::Quaternion>(c, pField->name);
            auto aVec = new float[4] {vec.x, vec.y, vec.z, vec.w};
            INSPECTOR_FIELD_VEC(pField->name, aVec, InputFloat4);
          }
          else if (pField->type->name == "UnityEngine.Transform") {
            auto t = pClass->GetValue<UnityResolve::UnityType::Transform*>(c, pField->name);
            INSPECTOR_FIELD(pField->name, std::string("T[ " + BuildSLinkLocation(t) + " ]"));
          }
          else if (pField->type->name == "UnityEngine.GameObject") {
            auto go = pClass->GetValue<UnityResolve::UnityType::GameObject*>(c, pField->name);
            INSPECTOR_FIELD(pField->name, std::string("G[ " + BuildSLinkLocation(go->GetTransform()) + " ]"));
          }
          else {

            auto [pa, pc] = GetAssemblyAndClassFromType(pField->type);

            if (nullptr != pc && CheckIsValueTypeOf(pc) == false) {
              auto inst = pClass->GetValue<void*>(c, pField->name);
              //auto goTitle = title + ":" + pField->name;
              //printf("check %s\n", goTitle.c_str());

              if (BuildField(pc, pa, inst, pField->name) == false) {
                INSPECTOR_FIELD(pField->name, std::string("[ *Empty* ]"));
              }
            }
            else {
              INSPECTOR_FIELD(pField->name, std::string("[ *Undefined* ]"));
            }
          }
        }

        ImGui::Columns(1);
      }

      ImGui::Dummy(ImVec2(0.0f, 4.0f));

      if (pClass->parent == "Object" || pClass->parent == "Component" || pClass->parent == "MonoBehaviour" || pClass->parent == "Behaviour") {
        break;
      }

      auto [pa, pc] = GetAssemblyAndClassFromType(pClass->parent_type);
      pClass = pc;
    }
    ImGui::TreePop();
  }

  return true;
}

void InspectorWindow::BuildComponents() {
  if (UnityBlaBla::ValidateGameObject(m_pTransform)) {
    auto components = m_pTransform->GetGameObject()->GetComponents<Component*>(UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Component"));
    for (auto c : components) {

      //auto type = c->GetType();
      auto type = new UnityResolve::Type{ .address = UnityResolve::Invoke<void*>("il2cpp_class_get_type", c->Il2CppClass) };
      type->name = UnityResolve::Invoke<const char*>("il2cpp_type_get_name", type->address);
      type->reflection_name = UnityResolve::Invoke<const char*>("il2cpp_type_get_reflection_name", type->address);
      type->assembly_qualified_name = UnityResolve::Invoke<const char*>("il2cpp_type_get_assembly_qualified_name", type->address);

      auto [pa, pc] = GetAssemblyAndClassFromType(type);
      if (nullptr != pc) {
        BuildField(pc, pa, c, type->name);
      }
      /*
      for (const auto& pAssembly : UnityResolve::assembly) {
        for (const auto& pClass : pAssembly->classes) {
          if (pClass->GetType() == type) {
            BuildField(pClass, pAssembly, c, pClass->name);

            break;
          }
        }
      }
      */
    }
  }
  
}

void InspectorWindow::FirstCall() {
  ImGui::SetWindowSize({ fWidth, fHeight });
}

void InspectorWindow::MainLoop() {


  ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar;
  if (ImGui::Begin(("Inspector : " + m_sObjName + " : " + std::to_string(m_instanceId)).c_str(), &windowOpen, windowFlags)) {

    if (firstCall == false) {
      firstCall = true;
      FirstCall();
    }

    BuildComponents();
  }

  ImGui::End();
}

InspectorWindow::InspectorWindow() : Window("Inspector", false) {
  fWidth = 500;
  fHeight = 600;
  mainWindowFunctions.push_back(std::bind(&InspectorWindow::MainLoop, this));
}
