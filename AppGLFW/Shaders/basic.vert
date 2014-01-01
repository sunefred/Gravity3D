#version 420 core

layout( location = 0 ) in vec3 inPosition;
layout( location = 2 ) in vec3 inNormal;
layout( location = 8 ) in vec2 inTexCoord;


uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;


out gl_PerVertex {
	vec4 gl_Position;
};

out vec3 vertPosition;
out vec3 vertNormal;
out vec2 vertTexCoord;


void main()
{
	vertPosition = inPosition;
	vertNormal = inNormal;
	vertTexCoord = inTexCoord;
	//gl_Position = vec4( inPosition, 1 );
	gl_Position = projMatrix * viewMatrix * modelMatrix * vec4( inPosition, 1 );
}
