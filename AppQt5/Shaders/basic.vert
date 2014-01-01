#version 420 core

layout( location = 0 ) in vec3 inPosition;
layout( location = 2 ) in vec3 inNormal;


uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;


out gl_PerVertex {
	vec4 gl_Position;
};

vec3 outNormal;

void main()
{
	gl_Position = vec4( inPosition, 1 );
	outNormal = inNormal;
	//gl_Position = projMatrix * viewMatrix * modelMatrix * vec4( inPosition, 1 );
}
