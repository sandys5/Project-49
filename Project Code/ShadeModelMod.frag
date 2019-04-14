#version 330 compatibility
uniform sampler2D uImageOne;
uniform float uLightX;
uniform float uLightY;
uniform float uLightZ;
uniform float uModelX;
uniform float uModelY;
uniform float uModelZ;
uniform float uTol;
uniform float uDb;
uniform float uDc;
uniform float uDs;
uniform float uNb;
uniform float uNc;
uniform float uNs;
in vec3 vNormal;
in vec2 vMC;

vec3 PtL;
vec3 rgb;
vec3 mixture;
vec3 uLightXYZ = vec3 ( uLightX, uLightY, uLightZ );
vec3 uModelXYZ = vec3( uModelX, uModelY, uModelZ );
vec3 Luminance = vec3( .2125, .7154, 0.0721 );
vec3 Contrast = vec3( .5, .5, .5 );
vec3 Brightness = vec3( 0., 0., 0. );
void
main()
{
    vec3 MoonLight = texture2D( uImageOne, vMC ).rgb;
    vec3 MoonDark = MoonLight;
    MoonLight = (1. - uDb)*Brightness + uDb*MoonLight;
    MoonLight = (1. - uDc)*Contrast + uDc*MoonLight;
    MoonLight = (1. - uDs)*(Luminance*MoonLight) + uDs*MoonLight;
    MoonDark = (1. - uNb)*Brightness + uNb*MoonDark;
    MoonDark = (1. - uNc)*Contrast + uNc*MoonDark;
    MoonDark = (1. - uNs)*(Luminance*MoonDark) + uNs*MoonDark; 
    vec3 uLight = normalize( (uLightXYZ - uModelXYZ) );
    float horizon = dot( uLight, vNormal );
    float d = smoothstep( (-uTol), uTol, horizon );
        if( horizon <= 1. && horizon > 0. )
        {
            rgb = mix( MoonDark, MoonLight, d );
            gl_FragColor = vec4( rgb, 1. );
        }
        if( (horizon >= -1 && horizon <= 0.) )
        {
            rgb = mix( MoonDark, MoonLight, d );
            gl_FragColor = vec4( rgb, 1. );
        }
}