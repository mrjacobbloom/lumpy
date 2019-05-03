//
//  Generic Object class
//
#include "Object.h"

//
//  Constructor
//
Object::Object()
{
   show = true;
   //  Location
   x0 = y0 = z0 = 0;
   //  Rotation
   th = ny = nz = 0;
   nx = 1;
   //  Color
   r0 = g0 = b0 = a0 = 1;
   //  Texture
   tex = NULL;
}

//
//  Show object
//
void Object::setShow(bool on)
{
   show = on;
}

//
//  Object position
//
void Object::setTranslate(float x,float y,float z)
{
   x0 = x;
   y0 = y;
   z0 = z;
}

//
//  Object rotation
//
void Object::setRotate(float t,float dx,float dy,float dz)
{
   th = t;
   nx = dx;
   ny = dy;
   nz = dz;
}

//
//  Object color
//
void Object::setColor(Color rgb)
{
   r0 = rgb.r;
   g0 = rgb.g;
   b0 = rgb.b;
   a0 = rgb.a;
}

//
//  Object color
//
void Object::setColor(float r,float g,float b,float a = 1)
{
   r0 = r;
   g0 = g;
   b0 = b;
   a0 = a;
}

//
//  Apply transformation
//
void Object::useTransform(float dx,float dy,float dz)
{
   glTranslated(x0,y0,z0);
   glRotated(th,nx,ny,nz);
   glScaled(dx,dy,dz);
}

//
//  Apply default color
//
void Object::useColor()
{
   useColor(Color(r0,g0,b0,a0));
}

//
//  Apply ambient and diffuse color
//  Specular is set to white
//  Emission is set to black
//
void Object::useColor(Color c)
{
   useColor(c,c,Color(1,1,1),Color(0,0,0),16);
}

//
//  Apply colors
//
void Object::useColor(Color a,Color d,Color s,Color e,float Ns)
{
   glColor4fv(d.fv());
   glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT ,a.fv());
   glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE ,d.fv());
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,s.fv());
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,e.fv());
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,&Ns);
}

//
//  Set texture
//
void Object::setTexture(QString file)
{
   tex = new QOpenGLTexture(QImage(file).mirrored());
   tex->setMinificationFilter(QOpenGLTexture::Linear);
   tex->setMagnificationFilter(QOpenGLTexture::Linear);
}

//
//  Enable texture
//
void Object::EnableTex()
{
   if (tex)
   {
      tex->bind();
      glEnable(GL_TEXTURE_2D);
   }
}

//
//  Disable texture
//
void Object::DisableTex()
{
   if (tex) glDisable(GL_TEXTURE_2D);
}
