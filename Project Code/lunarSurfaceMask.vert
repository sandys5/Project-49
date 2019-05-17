
out vec2  vXY;

void main( )
{
	vXY = gl_Vertex.xy;

    	vec3 tnorm = normalize( gl_NormalMatrix * gl_Normal );
	vec3 LightPos = vec3( 5., 10., 10. );
	vec3 ECposition = vec3( gl_ModelViewMatrix * gl_Vertex );	

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
