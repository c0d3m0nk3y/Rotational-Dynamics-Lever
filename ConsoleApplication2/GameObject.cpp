#include "GameObject.h"

GameObject::GameObject(float x, float y, float z, float r, float g, float b, int type) {
  this->x = x;
  this->y = y;
  this->z = z;
  this->r = r;
  this->g = g;
  this->b = b;
  this->type = type;
}