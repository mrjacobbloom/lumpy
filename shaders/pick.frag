//  Set color.rg to texcord.st for picking

void main()
{
   gl_FragColor = vec4(gl_TexCoord[0].st, 1.0, 1.0);
}
