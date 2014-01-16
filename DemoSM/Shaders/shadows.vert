#version 420 core


// vertex attribute input
//
//	we use layout semantic for easy client code flow
//
layout( location = 0 ) in vec3 in_position;
layout( location = 2 ) in vec3 in_normal;
layout( location = 8 ) in vec2 in_texCoord;


// output blocks, built in and user defined
out gl_PerVertex {
	vec4 gl_Position;
};
out PerVertex {
	vec3 position;
	vec3 normal;
	vec2 texCoord;
	vec4 positionLgt;
} out_;


// user defined uniforms
uniform mat4 modelMatrix;
uniform mat4 camViewMatrix;
uniform mat4 camProjMatrix;
uniform mat4 lgtViewMatrix;
uniform mat4 lgtProjMatrix;


// application entry point
void main()
{
	mat4 camModelViewProj = camProjMatrix * camViewMatrix * modelMatrix;
	mat4 lgtModelViewProj = lgtProjMatrix * lgtViewMatrix * modelMatrix;
	
	out_.position = in_position;
	out_.normal = in_normal;
	out_.texCoord = in_texCoord;


	out_.positionLgt = lgtModelViewProj * vec4( in_position, 1 );

	gl_Position = camModelViewProj * vec4( in_position, 1 );
}
