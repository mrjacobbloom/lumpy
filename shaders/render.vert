//  Phong lighting
uniform sampler2D lumpMap;

varying float offset;

vec4 phong(vec3 normal)
{
   //  P is the vertex coordinate on body
   vec3 P = vec3(gl_ModelViewMatrix * gl_Vertex);
   //  N is the object normal at P
   vec3 N = normalize(gl_NormalMatrix * normal);
   //  Light Position for light 0
   vec3 LightPos = vec3(gl_LightSource[0].position);
   //  L is the light vector
   vec3 L = normalize(LightPos - P);
   //  R is the reflected light vector R = 2(L.N)N - L
   vec3 R = reflect(-L, N);
   //  V is the view vector (eye at the origin)
   vec3 V = normalize(-P);

   //  Diffuse light intensity is cosine of light and normal vectors
   float Id = max(dot(L,N) , 0.0);
   //  Shininess intensity is cosine of light and reflection vectors to a power
   float Is = (Id>0.0) ? pow(max(dot(R,V) , 0.0) , gl_FrontMaterial.shininess) : 0.0;

   //  Vertex color
   return gl_FrontMaterial.emission                         // Emission color
     +    gl_LightModel.ambient*gl_FrontMaterial.ambient    // Global ambient
     +    gl_FrontLightProduct[0].ambient                   // Light[0] ambient
     + Id*gl_FrontLightProduct[0].diffuse                   // Light[0] diffuse
     + Is*gl_FrontLightProduct[0].specular;                 // Light[0] specular
}

void main()
{
   vec4 lump = texture2D(lumpMap,gl_MultiTexCoord0.st);
   vec4 vert = gl_Vertex;
   vert.y += 3.5 * lump.r;

   vec3 normal = 2.0*lump.gba - 1.0;

   float edginess = max( // edges fade out
      (abs(gl_MultiTexCoord0.s*2.0-1.0)-0.9)*10.0,
      (abs(gl_MultiTexCoord0.t*2.0-1.0)-0.9)*10.0
   );
   gl_FrontColor = vec4(phong(normal).rgb, gl_Color.a - max(edginess,0.0));
   gl_Position = gl_ModelViewProjectionMatrix * vert;
   gl_TexCoord[0] = gl_MultiTexCoord0;
   offset = vert.y;
}
