#version 330 compatibility
in vec4 vColor;
in vec3 PtL;
in vec3 PtE;
in vec3 vNormal;

vec4 WHITE = vec4( 1., 1., 1., 1. );
void main()
{
    float uKa = .5;
    float uKd = 1.;
    float uKs = 0.;
    float uShininess = 0;
    vec4 uColor = vec4( .4,.4,.4,1. );
    vec3 Normal = gl_NormalMatrix * vNormal;
    vec3 Light = normalize(PtL);
    vec3 Eye = normalize(PtE);
    vec4 ambient = uKa *uColor;
    //How much of the normal is going into the direction of the light,
    //then from that scalar we can decide the intensity of the light.
    float d = max( dot( Normal,Light ), 0. );
    vec4 diffuse = uKd * d * uColor;
    float s = 0;
    if( dot( Normal, Light ) > 0. ){
        vec3 ref = normalize( 2.* Normal * dot( Normal,Light )-Light );
        s = pow( max( dot(Eye,ref), 0. ), uShininess );
    }
    vec4 specular = uKs * s * WHITE;
    gl_FragColor = vec4( ambient.rgb + diffuse.rgb + specular.rgb, 1. );
}