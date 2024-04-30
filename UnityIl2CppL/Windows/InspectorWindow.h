#pragma once

#include <string>
#include <map>

#include "Window.h"
#include "../UnityResolve.hpp"

class InspectorWindow : public Window {
private:
  std::map<void *, std::string> m_fieldComponentAddresses{};

  bool BuildField(UnityResolve::Class* klass, UnityResolve::Assembly* pAssembly, void* c, std::string title);
  void BuildComponents();

  bool firstCall = false;
  void FirstCall();


protected:
  void MainLoop();

public:

  UnityResolve::UnityType::Transform * m_pTransform = nullptr;
  std::string m_sObjName;
  uint32_t m_instanceId;

  InspectorWindow();
};

