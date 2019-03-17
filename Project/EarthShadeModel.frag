#version 330 compatibility
uniform sampler2D uImageOne;
uniform sampler2D uImageTwo;
uniform sampler2D uImageThree;
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
uniform float uDCloud;
uniform float uNCloud;
uniform int uDebright;
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
    vec3 Day = texture2D( uImageOne, vMC ).rgb;
    vec3 Night = texture2D( uImageTwo, vMC ).rgb;
    vec3 Clouds = texture2D( uImageThree, vMC ).rgb;
    Day = mix( Day, Clouds, uDCloud );
    Day = (1. - uDb)*Brightness + uDb*Day;
    Day = (1. - uDc)*Contrast + uDc*Day;
    Day = (1. - uDs)*(Luminance*Day) + uDs*Day;
    Night = mix( Night, Clouds, uNCloud );
    Night = (1. - uNb)*Brightness + uNb*Night;
    Night = (1. - uNc)*Contrast + uNc*Night;
    Night = (1. - uNs)*(Luminance*Night) + uNs*Night; 
    vec3 uLight = normalize( (uLightXYZ - uModelXYZ) );
    float horizon = dot( uLight, vNormal );
    float d = smoothstep( (-uTol), uTol, horizon );
    if( horizon <= 1. && horizon > 0. )
    {
        rgb = mix( Night, Day, d );
        gl_FragColor = vec4( rgb, 1. );
    }
    if( (horizon >= -1 && horizon <= 0.) )
    {
        rgb = mix( Night, Day, d );
        gl_FragColor = vec4( rgb, 1. );
    }
}