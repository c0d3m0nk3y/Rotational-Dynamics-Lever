#ifndef __gameobject_h_
#define __gameobject_h_

class GameObject {
public:
  GameObject(float x, float y, float z, float r, float g, float b, int type);

  static const int TYPE_LINE = 0;
  static const int TYPE_POINT = 1;
  static const int TYPE_FULCRUM = 2;

  float velocityX, velocityY, velocityZ;
  float x, y, z;
  float lx, ly, lz; // line end co-ords
  float r, g, b;
  double size;
  double mass;
  int type; // TYPE_
};

#endif    /* __gameobject_h__ */