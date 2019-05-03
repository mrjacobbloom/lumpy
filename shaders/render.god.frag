//  Set the fragment color
uniform vec2 cursor;
uniform float cursorRad;
//uniform sampler2D water;
uniform sampler2D sand;
uniform sampler2D grass;
uniform sampler2D rock;

varying float offset;

// yellow seems like a good cursor color
const vec4 cursorColor = vec4(1.0, 1.0, 0.0, 1.0);

#define transLength 0.5
vec4 lerp(vec4 color1, float _min, vec4 color2)
{
   return mix(color1, color2, clamp((offset - _min)/transLength, 0.0, 1.0));
}

void main()
{
   float distToCursor = distance(gl_TexCoord[0].st, cursor);
   float diff = distToCursor - cursorRad;
   if(0.0 < diff && diff < 0.005) { // Render the cursor circle
      gl_FragColor = cursorColor;
   } else {
      vec4 texColor =
         lerp( texture2D(sand, gl_TexCoord[0].st*2.0), 0.6,
         lerp( texture2D(grass,gl_TexCoord[0].st*3.0), 1.5,
         lerp( texture2D(rock, gl_TexCoord[0].st*2.0), 3.0,
               vec4(1.0)
         )));
      gl_FragColor = gl_Color * texColor;
   }
}
