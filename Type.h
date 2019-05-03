#ifndef TYPE_H
#define TYPE_H

//
//  Utitily Vec2, Vec3 and Color classes
//

//  Vec3
class Vec3
{
   public:
      float x,y,z;
      Vec3(void)
      {
         x = y = z = 0;
      }
      Vec3(float X,float Y,float Z)
      {
         x = X;
         y = Y;
         z = Z;
      }
};

//  Vec2
class Vec2
{
   public:
      float x,y;
      Vec2(void)
      {
         x = y = 0;
      }
      Vec2(float X,float Y)
      {
         x = X;
         y = Y;
      }
};

//  RGB based Color class
class Color
{
   private:
      float v[4];
   public:
      float r,g,b,a;
      Color()
      {
         r = 0;
         g = 0;
         b = 0;
         a = 1;
      }
      Color(float R,float G,float B,float A=1)
      {
         r = R;
         g = G;
         b = B;
         a = A;
      }
      Color(const Vec3& V)
      {
         r = V.x;
         g = V.y;
         b = V.z;
         a = 1;
      }
      float* fv()
      {
         v[0] = r;
         v[1] = g;
         v[2] = b;
         v[3] = a;
         return v;
      }
};

#endif
