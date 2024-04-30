#pragma once

#include "Window.h"
#include "../UnityResolve.hpp"

class HierarchyWindow : public Window {

private:
  void BuildHierarchy(UnityResolve::UnityType::Transform* t);

  bool firstCall = false;
  void FirstCall();

protected:
  void MainLoop();

public:
  HierarchyWindow(bool main = false);
};

