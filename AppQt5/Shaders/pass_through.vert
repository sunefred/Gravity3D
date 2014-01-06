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
} out_;


// user defined uniforms
uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;


// application entry point
void main()
{
	out_.position = in_position;
	out_.normal = in_normal;
	out_.texCoord = in_texCoord;
	gl_Position = vec4( in_position, 1 );
	//gl_Position = projMatrix * viewMatrix * modelMatrix
	//			  * vec4( in_position, 1 );
}
