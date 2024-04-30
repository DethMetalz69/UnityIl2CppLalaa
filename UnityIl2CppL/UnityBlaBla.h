#pragma once

#include <vector>
#include "UnityResolve.hpp"

class UnityBlaBla {


public:
  static std::vector<UnityResolve::UnityType::GameObject*> m_allGameObjects;
  static std::vector<UnityResolve::UnityType::GameObject*> m_rootGameObjects;

  static void UpdateAllObjects();

  static bool ValidateGameObject(UnityResolve::UnityType::GameObject* obj);
  static bool ValidateGameObject(UnityResolve::UnityType::Transform* obj);
};

