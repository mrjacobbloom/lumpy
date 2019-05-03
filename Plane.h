//
//  Plane class
//  The constructor sets the position, size and rotation
//  All parameters are assigned default values
//
#ifndef Plane_H
#define Plane_H

#include "Object.h"

class Plane: public Object
{
private:
   float sx,sy,sz;  //  Dimensions in X/Y/Z directions
public:
   Plane(const QString tex="", float _rad=2.6); //  Constructor
   void setScale(float dx,float dy,float dz);  //  Set scale
   void display();                             //  Render the Plane
   float rad;                                  //  Radius of plane
};

#endif
