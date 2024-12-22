#include "CollisionManager.h"

CollisionManager::~CollisionManager(){}

void CollisionManager::CheckCollision(){}

void CollisionManager::ResetColliderList(){
    colliders_.clear();
}

void CollisionManager::AddCollider(Collider* object){
    colliders_.push_back(object);
}

void CollisionManager::AddCollider(std::list<Collider*> objects){
    colliders_.insert(colliders_.end(), objects.begin(), objects.end());
}
