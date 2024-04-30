#include "UnityBlaBla.h"


typedef UnityResolve::UnityType::GameObject GameObject;
typedef UnityResolve::UnityType::Transform Transform;
typedef UnityResolve::UnityType::UnityObject UnityObject;
typedef UnityResolve::UnityType::Component Component;


std::vector<GameObject*> UnityBlaBla::m_allGameObjects{};
std::vector<GameObject*> UnityBlaBla::m_rootGameObjects{};


void UnityBlaBla::UpdateAllObjects() {
  /*
  try {
    auto assembly = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Transform");
    auto method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Object")->Get<UnityResolve::Method>("FindObjectsOfType", { "System.Type" });

    if (assembly && method) {
      printf("exec  >  ");
      try {
        auto arr = method->Invoke<UnityResolve::UnityType::Array<GameObject*>*>(assembly->GetType());
        printf("size %d\n", arr->max_length);
      }
      catch (...) {

      }
    }


  }
  catch (...) {

  }
  */

  /*
  m_allGameObjects.clear();
  m_rootGameObjects.clear();

  try {
    auto scene = UnityResolve::UnityType::SceneManager::GetActiveScene();
    //printf("name %s root count %d\n", scene.GetName().c_str(), scene.GetRootCount());

    auto vec = scene.GetRootGameObjects();
    m_rootGameObjects.insert(m_rootGameObjects.end(), vec.begin(), vec.end());

  } 
  catch (...) {

  }
  */


  m_allGameObjects.clear();
  m_rootGameObjects.clear();

  try {
    auto allObjects = UnityObject::FindObjectsOfType<Transform>();
    for (auto obj : allObjects) {
      if (nullptr == obj->GetParent()) {
        m_rootGameObjects.push_back(obj->GetGameObject());
      }
    }
  } 
  catch (...) {

  }

  for (auto obj : m_allGameObjects) {
    //
    //auto t = obj->GetTransform();
    //

    /*
    auto root = obj->GetTransform()->GetRoot()->GetGameObject();
    bool found = false;
    for (auto robj : m_rootGameObjects) {
      if (root == robj) {
        found = true;
        break;
      }
    }

    if (found == false) {
      m_rootGameObjects.push_back(root);
      //std::cout << "obj " << root->GetName() << std::endl;
    }
    */
    //std::cout << "count " << root->GetTransform()->GetChildCount() << std::endl;
  }
  
}

bool UnityBlaBla::ValidateGameObject(GameObject* obj) {
  if (obj == nullptr) {
    return false;
  }

  /*
  for (auto o : m_allGameObjects) {
    if (o == obj) {
      return true;
    }
  }
  */

  return true;
}

bool UnityBlaBla::ValidateGameObject(Transform* t) {
  if (t == nullptr) {
    return false;
  }

  auto obj = t->GetGameObject();

  return ValidateGameObject(obj);
}
