#version 330 compatibility

const float PI = 3.14159;

in vec2  vXY;
uniform float uAlpha;
uniform float uRadius;

void
main( )
{


	float area = PI*uRadius*uRadius;
	vec3 newcolor = vec3(.7,.7,.7);

	float s = (vXY.x +127.75)/255.5;

	float t = (vXY.y +127.75)/255.5;

	float Scenter = s-.5;
	float Tcenter = t-.5;

	float varS = Scenter*Scenter;
	float varT = Tcenter*Tcenter;

	if(varS+varT <= uRadius*uRadius){
		gl_FragColor = vec4(newcolor, 1.);
	}
	else{
		gl_FragColor = vec4(newcolor, uAlpha-(varS+varT));

	}	
}
