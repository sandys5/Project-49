#version 330 compatibility

out vec2 vMC;
out vec3 vNormal;
void
main()
{
    vMC = gl_MultiTexCoord0.st;
    vNormal = normalize( gl_Normal );
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}