#version 330 compatibility

out vec4 vColor;
out vec3 PtL;
out vec3 PtE;
out vec3 vNormal;
uniform float uLightX;
uniform float uLightY;
uniform float uLightZ;
uniform float uLunarX;
uniform float uLunarY;
uniform float uLunarZ;

vec3 LightXYZ = vec3( uLightX, uLightY, uLightZ );
vec3 LM_XYZ = vec3( uLunarX, uLunarY, uLunarZ );

void main()
{
    //Eye Position
    vec4 ECPosition = gl_ModelViewMatrix*gl_Vertex;
    //Point to light
    PtL = LightXYZ - ECPosition.xyz;
    //Point to Eye
    PtE = LM_XYZ  - ECPosition.xyz;
    vNormal = gl_Normal;
    vColor = gl_Color;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}