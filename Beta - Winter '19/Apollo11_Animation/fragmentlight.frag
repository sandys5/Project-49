#version 330 compatibility
in vec4 vColor;
in vec3 PtL;
in vec3 PtE;
in vec3 vNormal;
uniform float AmbientR;
uniform float AmbientG;
uniform float AmbientB;
uniform float DiffuseR;
uniform float DiffuseG;
uniform float DiffuseB;
uniform float SpecularR;
uniform float SpecularG;
uniform float SpecularB;
uniform float dissolve;
uniform float specExp;

vec4 AmbientRGB = vec4( AmbientR, AmbientG, AmbientB, 1.);
vec4 DiffuseRGB = vec4( DiffuseR, DiffuseG, DiffuseB, 1.);
vec4 SpecularRGB = vec4( SpecularR, SpecularG, SpecularB, 1.);
vec4 WHITE = vec4( 1., 1., 1., 1. );
void main()
{
    float uKa = 1.;
    float uKd = 1.;
    float uKs = 0.;
    float uShininess = 0;
    vec4 uColor = vec4( .4,.4,.4,1. );
    vec3 Normal = gl_NormalMatrix * vNormal;
    vec3 Light = normalize(PtL);
    vec3 Eye = normalize(PtE);
    vec4 ambient = uKa *AmbientRGB;
    //How much of the normal is going into the direction of the light,
    //then from that scalar we can decide the intensity of the light.
    float d = max( dot( Normal,Light ), 0. );
    vec4 diffuse = uKd * d * DiffuseRGB;
    float s = 0;
    if( dot( Normal, Light ) > 0. ){
        vec3 ref = normalize( 2.* Normal * dot( Normal,Light )-Light );
        s = pow( max( dot(Eye,ref), 0. ), specExp );
    }
    vec4 specular = uKs * s * SpecularRGB;
    gl_FragColor = vec4( ambient.rgb + diffuse.rgb + specular.rgb, dissolve);
}