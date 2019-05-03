//  Set the fragment color

uniform sampler2D prevTex; // The previous lump map texture
uniform vec2 cursor;
uniform float cursorRad;

float weight(float v)
{
   return max(0.0, 1.0 - (v / cursorRad));
}

// from https://codepen.io/zadvorsky/pen/qNdrmR
float easeQuadInOut(float t)
{
   float p = 2.0 * t * t;
   return t < 0.5 ? p : -p + (4.0 * t) - 1.0;
}

float calcRAt(vec2 loc)
{
   float r = texture2D(prevTex,loc).r;
   float dist = weight(length(cursor - gl_TexCoord[0].st));
   return r + 0.2*cursorRad*easeQuadInOut(dist);
}

//
// Approximate the lump at a neighboring point, for calculating normals
//
vec3 lump(float dx, float dz)
{
   return vec3(dx, 3.5*calcRAt(gl_TexCoord[0].st + vec2(dx,dz)), dz);
}

vec3 getNormal(vec3 p1, vec3 p2, vec3 p3)
{
   vec3 dir = cross((p2-p1),(p3-p1));
   if(dir.y < 0.0) dir += -1.0;
   return dir;
}

#define d 0.01
//
// Calculate the normals for virtual faces around the vertex and average them
//
vec3 getSmoothNormal()
{
   vec3 vert = lump(0.0,0.0);

   vec3 total = vec3(0.0);
   total += getNormal(vert, lump( +d,0.0), lump(0.0, -d));
   total += getNormal(vert, lump(0.0, -d), lump( -d,0.0));
   total += getNormal(vert, lump( -d,0.0), lump(0.0, +d));
   total += getNormal(vert, lump(0.0, +d), lump( +d,0.0));
   return normalize(total);
}

void main()
{
   vec3 normal = 0.5*getSmoothNormal() + 0.5;
   gl_FragColor = vec4(calcRAt(gl_TexCoord[0].st), normal);
}
