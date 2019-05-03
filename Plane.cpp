//
//  Plane class
//
#include "Plane.h"

//
//  Constructor
//
Plane::Plane(const QString tex, float _rad)
{
   rad = _rad;
   sx = sy = sz = 1;
   if (tex.length()>0) setTexture(tex);
}

//
//  Set scaling
//
void Plane::setScale(float dx,float dy,float dz)
{
   sx = dx;
   sy = dy;
   sz = dz;
}

//
//  Display the Plane
//
void Plane::display()
{
   if (!show) return;
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   useTransform(sx,sy,sz);
   //  Texture on
   EnableTex();
   //  Front
   useColor();

   float step = rad / 128.0;

   for(float z = -rad; z <= rad; z += step) {
      glBegin(GL_QUAD_STRIP);
      for(float x = -rad; x < rad; x += step) {
         glNormal3f(0, 1, 0);
         glTexCoord2f((x+rad)/(2.0*rad),(z     +rad)/(2*rad)); glVertex3f(x,0.0, z);
         glTexCoord2f((x+rad)/(2.0*rad),(z+step+rad)/(2*rad)); glVertex3f(x,0.0, z+step);
      }
      glEnd();
   }
   //  Texture off
   DisableTex();
   //  Undo transofrmations
   glPopMatrix();
}
