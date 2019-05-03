uniform   float time;  //  Time
uniform sampler2D lumpMap;
attribute float Start; //  Start time

void main(void)
{
   //  Particle location
   vec4 vert = gl_Vertex;
   //  Time offset mod 5
   float t = mod(time+Start,2.0);
   float lumpHeight = texture2D(lumpMap,gl_MultiTexCoord0.st).r * 3.5;
   
   if(t < 2.0 && lumpHeight > 2.7)
   {
      vert.y -= t;
      gl_FrontColor = vec4(1.0);
   }
   else
   {
      gl_FrontColor = vec4(0.0);
   }

   //  Transform particle location
   gl_Position = gl_ModelViewProjectionMatrix*vert;
}
